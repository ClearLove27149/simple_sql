#include "table.h"

// Row <<
std::ostream& operator<< (std::ostream& out, const Row& row) {
    char buf[1024] = {0,};
    sprintf(buf, "{id=%d,username=%s,email=%s}", row.id, row.username, row.email);
    out << std::string(buf);
    return out;
}

void serialize_row(Row &row, void* destination) {
    memcpy((char*)destination + ID_OFFSET, &(row.id), ID_SIZE);
    memcpy((char*)destination + USERNAME_OFFSET, &(row.username), USERNAME_SIZE);
    memcpy((char*)destination + EMAIL_OFFSET, &(row.email), EMAIL_SIZE);
    //std::cout << *((int*)destination + ID_OFFSET) << std::endl;
    //std::cout << *((char*)destination + USERNAME_OFFSET) << *((char*)destination + USERNAME_OFFSET + 1) << std::endl;
}

void unserialize_row(Row &row, void* source) {
    memcpy(&(row.id), (char*)source + ID_OFFSET, ID_SIZE);
    memcpy(&(row.username), (char*)source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(row.email), (char*)source + EMAIL_OFFSET, EMAIL_SIZE);
    //std::cout << *((int*)source + ID_OFFSET) <<std::endl;
    //std::cout << *((char*)source + USERNAME_OFFSET) << *((char*)source + USERNAME_OFFSET + 1) << std::endl;
}


// Pager
Pager::Pager(const char* filename) {
    //std::cout<<"filename: "<<filename<<std::endl;
    // access() 判断文件是否存在,return 0 表示存在
    //file_descriptor = fopen(filename, "w+");
    /** 需要fopen 重写
    if (access(filename, F_OK) == 0) {
        chmod(filename, _O_ACCMODE);
        file_descriptor = open(filename, O_RDWR |     // Read/Write mode
                                         O_CREAT, // Create file if it does not exist
                                         _O_ACCMODE);  // User read permission
    } else {
        file_descriptor = open(filename, O_RDWR | O_CREAT,    // Read/Write mode
                                        _O_ACCMODE);  // User read permission
    }
    **/
#if defined(_WIN32) || defined(_WIN64)
    file_descriptor = fopen(filename, "a+");
    if (file_descriptor == NULL) {
        printf_s("Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    // move cursor
    //rewind(file_descriptor);
    fseek(file_descriptor, 0, SEEK_END);
    file_length = ftell(file_descriptor);
#else
    file_descriptor = open(filename, O_RDWR |     // Read/Write mode
                                    O_CREAT, // Create file if it does not exist
                                    0x00400 | 0x00200);  // User read/w permission

    if (file_descriptor < 0) {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    file_length = lseek(file_descriptor, 0, SEEK_END);
#endif // defined

    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pages[i] = nullptr;
    }
}

void* Pager::get_page(uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES)
    {
        std::cout << "Tried to fetch page number out of bounds. " << page_num << " > "
                  << TABLE_MAX_PAGES << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pages[page_num] == nullptr) {
        //内存中没有page这一页，分配内存
        void* page = malloc(PAGES_SIZE);

        uint32_t num_pages = file_length / PAGES_SIZE;
        // 保存不完整的最后一页
        if (file_length % PAGES_SIZE) {
            num_pages++;
        }
        if (page_num <= num_pages) {
#if defined(_WIN32) || defined(_WIN64)
            fseek(file_descriptor, page_num * PAGES_SIZE, SEEK_SET);
            size_t bytes_read = fread(page, 1, PAGES_SIZE, file_descriptor);
#else
            lseek(file_descriptor, page_num * PAGES_SIZE, SEEK_SET);
            size_t bytes_read = read(file_descriptor, page, PAGES_SIZE);
#endif // defined

            if (bytes_read == -1)
            {
                std::cout << "Error reading file: " << errno << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        pages[page_num] = page;
    }
    return pages[page_num];
}
void Pager::pager_flush(uint32_t page_num, uint32_t size) {
    if (pages[page_num] == nullptr)
    {
        std::cout << "Tried to flush null page" << std::endl;
        exit(EXIT_FAILURE);
    }
#if defined(_WIN32) || defined(_WIN64)
    off_t offset = fseek(file_descriptor, page_num * PAGES_SIZE, SEEK_SET);
#else
    off_t offset = lseek(file_descriptor, page_num * PAGES_SIZE, SEEK_SET);
#endif // defined

    if (offset == -1)
    {
        std::cout << "Error seeking: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
#if defined(_WIN32) || defined(_WIN64)
    size_t bytes_written = fwrite(pages[page_num], 1, size, file_descriptor);
#else
    size_t bytes_written = write(file_descriptor, pages[page_num], size);
#endif // defined

    if (bytes_written == -1)
    {
        std::cout << "Error writing: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
}
// Table
Table::~Table() {
    uint32_t num_full_pages = num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++)
    {
        if (pager.pages[i] == nullptr)
        {
            continue;
        }
        pager.pager_flush(i, PAGES_SIZE);
        free(pager.pages[i]);
        pager.pages[i] = nullptr;
    }

    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    uint32_t num_additional_rows = num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0)
    {
        uint32_t page_num = num_full_pages;
        if (pager.pages[page_num] != nullptr)
        {
            pager.pager_flush(page_num, num_additional_rows * ROW_SIZE);
            free(pager.pages[page_num]);
            pager.pages[page_num] = nullptr;
        }
    }
#if defined(_WIN32) || defined(_WIN64)
    int result = fclose(pager.file_descriptor);
#else
    int result = close(pager.file_descriptor);
#endif // defined

    if (result == -1)
    {
        std::cout << "Error closing db file." << std::endl;
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        void *page = pager.pages[i];
        if (page)
        {
            free(page);
            pager.pages[i] = nullptr;
        }
    }
}
