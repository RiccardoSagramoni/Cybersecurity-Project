#include <openssl/rand.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <limits>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <openssl/dh.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#define LENGTH 2048

using namespace std;


class Client {
    int sockfd = 0;
    string name;
    sockaddr_in server_addr;
    string ip = "127.0.0.1";
    const uint16_t port;

public:
    Client(const uint16_t _port, const string& _name);
    ~Client();
    bool configure_socket();
    bool connects();
    void exit();
    void str_overwrite_stdout();
    void str_trim_lf (char* arr, int length);
    sockaddr_in get_server_addr();
    int get_sock();

    string get_username ();
};

class ClientThread {
	Client* client;
	int main_server_socket;
	sockaddr_in main_server_address;
	const string filename_prvkey = "rsa_privkey.pem";
public:
	ClientThread(Client* cli, const int socket, const sockaddr_in addr);
	void run();
    static int send_message (const int socket, void* msg, const uint32_t msg_len);
	static long receive_message (const int socket, void** msg);
    DH* get_dh2048();
    int negotiate(const string& username);
    EVP_PKEY* generate_key_dh();
	EVP_PKEY* get_client_private_key();
	unsigned char* encrypt_message (unsigned char* msg, size_t msg_len, unsigned char* key, size_t key_len, unsigned char* iv, size_t& ciphertext_len);
    int receive_from_server_pub_key(EVP_PKEY*& peer_key);
    const EVP_CIPHER* get_symmetric_cipher();
	unsigned char* decrypt_message(unsigned char* chipertext, size_t chipertext_len, unsigned char* key, size_t key_len, unsigned char* iv, size_t& plainlen);
	unsigned char* sign_message(unsigned char* msg, size_t msg_len, unsigned int& signature_len);
	int send_sig(EVP_PKEY* my_dh_key,EVP_PKEY* peer_key, unsigned char* shared_key, size_t shared_key_len, unsigned char* iv);
	int get_sig(unsigned char* ciphertext, size_t ciphertext_len, EVP_PKEY* my_dh_key,EVP_PKEY* peer_key, unsigned char* shared_key, size_t shared_key_len, unsigned char* iv);
	int verify_server_signature(unsigned char* signature, size_t signature_len, unsigned char* cleartext, size_t cleartext_len, EVP_PKEY* client_pubkey);
    unsigned char* derive_session_key(EVP_PKEY* my_dh_key, EVP_PKEY* peer_key, size_t key_len);

    void secure_free (void* addr, size_t len);
};

