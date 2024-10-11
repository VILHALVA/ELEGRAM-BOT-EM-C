#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "Config.h"

void sendMessage(const char* chat_id, const char* text) {
    CURL *curl;
    CURLcode res;

    char url[256];
    snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/sendMessage", TOKEN);

    json_t *message = json_object();
    json_object_set_new(message, "chat_id", json_string(chat_id));
    json_object_set_new(message, "text", json_string(text));

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_dumps(message, 0));

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    json_decref(message);
}

void processUpdate(json_t *update) {
    json_t *message = json_object_get(update, "message");
    if (message) {
        const char *chat_id = json_string_value(json_object_get(message, "chat").id));
        const char *command = json_string_value(json_object_get(message, "text"));

        if (strcmp(command, "/start") == 0) {
            char response[256];
            snprintf(response, sizeof(response), "Olá! Eu sou um bot. Use /help para ver os comandos disponíveis.");
            sendMessage(chat_id, response);
        } 
        else if (strcmp(command, "/help") == 0) {
            sendMessage(chat_id, "/start - Exibe a saudação\n/help - Exibe esta mensagem de ajuda\n/info - Exibe informações sobre o bot");
        } 
        else if (strcmp(command, "/info") == 0) {
            sendMessage(chat_id, "Eu sou um bot criado para ajudar com tarefas diversas no Telegram.");
        } 
        else {
            sendMessage(chat_id, "Comando desconhecido. Por favor, tente outro comando.");
        }
    }
}

void getUpdates(long offset) {
    CURL *curl;
    CURLcode res;

    char url[256];
    snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/getUpdates?offset=%ld", TOKEN, offset);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
}

int main() {
    long offset = 0;

    while (1) {
        json_t *updates = getUpdates(offset);
        json_t *results = json_object_get(updates, "result");

        for (size_t i = 0; i < json_array_size(results); i++) {
            json_t *update = json_array_get(results, i);
            processUpdate(update);
            offset = json_integer_value(json_object_get(update, "update_id")) + 1;
        }

        sleep(1); 
    }

    return 0;
}
