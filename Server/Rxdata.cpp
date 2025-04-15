#include "Rxdata.h"

// When receive data, print console and make echo Func
char* print(vector<char>* str) {
    int p = 0;
    char out[BUFFERSIZE + 1];
    char* ret = new char[str->size() + 10];
    memcpy(ret, "echo - ", 7);
    cout << "From Client message : ";
    
    for (int n = 0; n < (str->size() / BUFFERSIZE) + 1; n++) {
        int size = str->size();
        if (size > BUFFERSIZE) {
            if (str->size() < (n + 1) * BUFFERSIZE) {
                size = str->size() % BUFFERSIZE;
            } else {
                size = BUFFERSIZE;
            }
        }
        for (int i = 0; i < size; i++, p++) {
            out[i] = *(str->begin() + p);
            if (out[i] == '\0') {
                out[i] = ' ';
            }
            *(ret + p + 7) = out[i];
        }
        out[size] = '\0';
        cout << out;
    }
    cout << endl;
    memcpy(ret + p + 7, "\n>\0", 3);
    return ret;
}
