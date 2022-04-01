#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned int data_type_size;
    char* data;
    char* horizontal_parity;
    char* vertical_parity;
} Payload;

int power(int number, int exponent) {
    if (exponent == 0) {
        return 1;
    } else {
        for (int i = 1; i < exponent; i++) {
            number *= number;
        }
        return number;
    }
}

int string_length(char* data) {
    for (int i = 0;; i++) {
        if (data[i] == '\0') {
            return i;
        }
    }
}

char* generate_vertical_parity(unsigned int size, char* data) {
    char* final_string = malloc(size);

    // Iteração pelas linhas
    for (int i = 0; i < size; i++) {
        unsigned char sum_of_ones = 0;
        // Iteração pelas colunas
        for (int j = 0; j < string_length(data) / size; j++) {
            if (data[i + size * j] == '1') {
                sum_of_ones++;
            }
        }

        if (sum_of_ones % 2) {
            final_string[i] = '1';
        } else {
            final_string[i] = '0';
        }
    }

    printf("Paridade vertical: %s\n", final_string);
    return final_string;
}

char* generate_horizontal_parity(unsigned int size, char* data) {
    unsigned char* final_string = malloc(string_length(data) / size);
    unsigned char sum_of_ones = 0;
    for (int i = 0; i < string_length(data); i++) {
        if (data[i] == '1') {
            sum_of_ones++;
        }

        // Insere a soma de uns na string final, após verificar cada palavra
        if ((i + 1) % size == 0) {
            if (sum_of_ones % 2) {
                final_string[i / size] = '1';
            } else {
                final_string[i / size] = '0';
            }
            sum_of_ones = 0;
        }
    }

    printf("Paridade horizontal: %s\n", final_string);
    return final_string;
}

Payload correct_data(Payload payload) {
    char* current_data_horizontal_parity = generate_horizontal_parity(payload.data_type_size, payload.data);
    char* current_data_vertical_parity = generate_vertical_parity(payload.data_type_size, payload.data);

    // Iteração pelas linhas
    for (int i = 0; i < string_length(payload.data) / payload.data_type_size; i++) {
        // Comparação de paridades horizontais
        if (current_data_horizontal_parity[i] != payload.horizontal_parity[i]) {
            int occurences = 0;
            int occurence_index = 0;
            // Iteração pelas colunas,
            for (int j = 0; j < payload.data_type_size; j++) {
                // Comparação de paridades verticais
                if (current_data_vertical_parity[j] != payload.vertical_parity[j]) {
                    occurences++;
                    occurence_index = j;
                }
            }

            // Não detecta duas ocorrências por causa de ambiguidade
            if (occurences == 1) {
                unsigned int corrected_data_length = sizeof(char) * (string_length(payload.data) + 1);
                // Alocação de novo datagrama
                char* corrected_data = malloc(corrected_data_length);
                for (int k = 0; k < corrected_data_length + 1; k++) {
                    if (k == i + occurence_index) {
                        if (payload.data[k] == '1') {
                            corrected_data[k] = '0';
                        } else {
                            corrected_data[k] = '1';
                        }
                    } else if (k == corrected_data_length) {
                        // Terminação em null
                        corrected_data[k] = '\0';
                    } else {
                        corrected_data[k] = payload.data[k];
                    }
                }
                payload.data = corrected_data;
            }
        }
    }

    return payload;
}

int main() {
    char* dado_inicial = "01101100011000010111001101100001";
    char* dado_alterado = "01101110011000010111001101100001";
    printf("Enviado:\n");
    Payload payload = {8, dado_inicial};
    payload.horizontal_parity = generate_horizontal_parity(payload.data_type_size, payload.data);
    payload.vertical_parity = generate_vertical_parity(payload.data_type_size, payload.data);

    payload.data = dado_alterado;

    printf("\nRecebido:\n");
    Payload correct_payload = correct_data(payload);

    printf("\n  Dado enviado: %s\n", dado_inicial);
    printf(" Dado recebido: %s\n", payload.data);
    printf("Dado corrigido: %s\n", correct_payload.data);

    return 0;
}
