#include "FileIOInterface.h"
// Writes a file, returns true on success.
class FileIO : public FileIOInterface {
public:
  virtual FILE* Open(const char* filename, const char* mode) {
    return fopen(filename, mode);
  }

  virtual size_t Write(const void* data, size_t size, size_t num, FILE* file) {
    return fwrite(data, size, num, file);
  }

  virtual int Close(FILE* file) {
    return fclose(file);
  }
};

int main() {
	FileIO *file = new FileIO;
	WriteFile(file, "test.txt", "12332", 4);
	return 0;
}
