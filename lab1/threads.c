//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: lab 1, QUESTION 3: threads
//
// REMARKS: What is the purpose of this program?
// use threads to download a file from the internet and show “progress” concurrently
//-----------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <curl/curl.h>

//global variables
int flag = 1;
static FILE *output; // make sure to `fopen` and `fclose` this in `main`!
size_t write_data(char *, size_t, size_t, void *);
CURL *curl;


//------------------------------------------------------
// write_data()
//
// PURPOSE: get from lab1 description's link
// INPUT PARAMETERS:
//           data
//           size
//           nmemb
//           extra
// OUTPUT PARAMETERS:
//           size_t
//------------------------------------------------------
size_t write_data(char *data, size_t size, size_t nmemb, void *extra) {
    (void) extra; // we're not using this argument at all, cast it
    // to explicitly tell the compiler that we know it's unused.
    return fwrite(data, size, nmemb, output);
}

//------------------------------------------------------
// graphic()
//
// PURPOSE: show the process
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
static void *graphic() {
    while(flag){
        //to print process
        for (int i = 0; i < 100; ++i) {
            //generate different symbols
            int sign = rand()%93 + 33;
            printf("%c",sign);
            fflush(stdout);
            usleep(10000);
        }
        printf("\n");
    }
    return NULL;
}

//------------------------------------------------------
// download()
//
// PURPOSE: download the file
// INPUT PARAMETERS:
//
// OUTPUT PARAMETERS:
//
//------------------------------------------------------
static void *download() {
    sleep(1);
    if (curl) {
        CURLcode res;

        // change this to whatever URL you actually want to download, being
        // careful to not download something bigger than the `quota` command
        // reports as the maximum space you have in your user directory.
        res = curl_easy_setopt(curl, CURLOPT_URL, "https://google.com");
        assert(res == CURLE_OK);
        // use our write function (to write to a file instead of stdout)
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        assert(res == CURLE_OK);
        // artificially limit download speeds to 2kb/s
        res = curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t) 2048);
        assert(res == CURLE_OK);
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
        assert(res == CURLE_OK);
        res = curl_easy_perform(curl);
        assert(res == CURLE_OK);
        curl_easy_cleanup(curl);
        flag = 0;
    }

    return NULL;
}


//------------------------------------------------------
// main()
//
// PURPOSE: the entrance of my code
// INPUT PARAMETERS:
//     argc: the numbers of argv
//     argv: the input
// OUTPUT PARAMETERS:
//      status code
//------------------------------------------------------
int main() {

    curl = curl_easy_init();
    //open the file
    output = fopen("downloadFile", "wb");
    curl_global_init(CURL_GLOBAL_ALL);
    //to store 2 threads
    pthread_t p[2];
    //create those threads with different jobs
    pthread_create(&p[0], NULL, graphic, NULL);
    pthread_create(&p[1], NULL, download, NULL);

    //wait for children process ends
    for (int i = 0; i < 2; i++) { //wait for all threads ending
        pthread_join(p[i], NULL);
    }

    //close the file
    fclose(output);
    return 0;

}
