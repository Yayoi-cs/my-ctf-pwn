#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<zlib.h>
#include<alloca.h>

typedef struct reqData {
    unsigned char *str;
    unsigned char *res;
    size_t len;
    size_t resLen;
    struct reqData *previous;
    struct reqData *next;
} reqData;

reqData *start = NULL;

void statusImATeapot(void) { // I love 418 I'm a teapot!!
    puts("418 I'm a teapot");
    exit(1);
}

void win(void) {
    system("/bin/cat flag*");
    exit(0);
}

void *m_alloc(void * pf, unsigned int item, unsigned int size){
    void *result = malloc(item * size);
    return result;
}

void *m_free(void *pf, void *p){
    free(p);
}

unsigned char* bodyInputer(unsigned char len) {
    len++;
    unsigned char *str = (unsigned char *)malloc(len * sizeof(unsigned char));
    if (str == NULL) statusImATeapot();
    fgets(str,len+1,stdin);
    return str;
}

void responseWriter(unsigned char *res, size_t len) {
    for (size_t i = 0;i < len; ++i) {
        printf("\\x%02x",res[i]);
    }
    puts("");
}

void myDecompress(reqData *data) {
    z_stream *stream = (z_stream *)alloca(sizeof(z_stream));

    stream->zalloc = m_alloc;
    stream->zfree = m_free;

    if (inflateInit(stream) != Z_OK) statusImATeapot();
    stream->next_in = (unsigned char *)data->res;
    stream->avail_in = (uInt)data->resLen;
    stream->next_out = (unsigned char *)data->str;
    stream->avail_out = (uInt)data->len;

    if (inflate(stream, Z_FINISH) != Z_STREAM_END) {
        inflateEnd(stream);
        statusImATeapot();
    }
    inflateEnd(stream);
}

void myCompress(unsigned char *str, unsigned char len, unsigned char **res, size_t *resLen) {
    z_stream *stream = (z_stream *)alloca(sizeof(z_stream));

    stream->zalloc = m_alloc;
    stream->zfree = m_free;

    if (deflateInit(stream,Z_BEST_COMPRESSION) != Z_OK) statusImATeapot();
    stream->next_in = (unsigned char* )str;
    stream->avail_in = (uInt)len;
    size_t tmpLen = deflateBound(stream,(uLong)len);
    *res = (unsigned char *)malloc(tmpLen);
    if (*res == NULL) statusImATeapot();
    stream->next_out = *res;
    stream->avail_out = tmpLen;

    if (deflate(stream, Z_FINISH) != Z_STREAM_END) {
        free(*res);
        deflateEnd(stream);
        statusImATeapot();
    }
    *resLen = stream->total_out;

    deflateEnd(stream);
}

void myDeflate(void) {
    unsigned char len;
    puts("[deflate]Input the length");
    scanf("%hhu%*c",&len);
    unsigned char *str = bodyInputer(len);
    unsigned char *res = NULL;
    size_t resLen = 0;
    myCompress(str,len,&res,&resLen);
    responseWriter(res,resLen);
    reqData *new = (reqData *)malloc(sizeof(reqData));
    new->res = res;
    new->str = str;
    new->len = len;
    new->resLen = resLen;
    //Add data but I don't want to comsume the memory!
    if (start == NULL) {
        new->next = NULL;
        new->previous = NULL;
        start = new;
        return;
    }
    new->next = start;
    while(new->next->next != NULL) {
        new->next = new->next->next;
    }
    new->next->next = new;
    new->previous = new->next;
    new->next = NULL;
}

void myInflate(void) {
    int index;
    puts("[inflate]Input the index");
    scanf("%d",&index);
    reqData *tmp = start;
    for (int i = 0;i < index;i++) {
        if (tmp->next == NULL) statusImATeapot();
        tmp = tmp->next;
    }   
    myDecompress(tmp);
}

void get(void) {
    if (start == NULL) statusImATeapot();
    int index;
    puts("[get]Input the index");
    scanf("%d",&index);
    reqData *tmp = start;
    for (int i = 0;i < index;i++) {
        if (tmp->next == NULL) statusImATeapot();
        tmp = tmp->next;
    }
    responseWriter(tmp->str,tmp->len);
    puts("->");
    responseWriter(tmp->res,tmp->resLen);
}

void delete(void) {
    if (start == NULL) statusImATeapot();
    int index;
    puts("[delete]Input the index");
    scanf("%d",&index);
    reqData *tmp = start;
    for (int i = 0;i < index;i++) {
        if (tmp->next == NULL) statusImATeapot();
        tmp = tmp->next;
    }
    if (index == 0) {
        start = tmp->next == NULL ? NULL : tmp->next;
        free(tmp->str);
        free(tmp->res);
        free(tmp);
    } else if (tmp->next == NULL) {
        tmp->previous->next = NULL;
        free(tmp->str);
        free(tmp->res);
        free(tmp);
    } else {
        tmp->previous->next = tmp->next;
        free(tmp->str);
        free(tmp->res);
        free(tmp);
    }
}

void menu(void) {
    puts("1.Deflate");
    puts("2.Inflate");
    puts("3.Get");
    puts("4.Delete");
    puts("5.Quit");
}

int main(void) {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    while(1) {
        menu();
        int choice;
        scanf("%d",&choice);
        switch (choice) {
        case 1:
            myDeflate();
            break;
        case 2:
            myInflate();
            break;
        case 3:
            get();
            break;
        case 4:
            delete();
            break;
        case 5:
            exit(0);
            break;
        default:
            break;
        }
    }
}
