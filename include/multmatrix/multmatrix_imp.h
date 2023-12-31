//
// Created by adria on 29/10/2023.
//

#ifndef MULTMATRIX_MULTMATRIX_IMP_H
#define MULTMATRIX_MULTMATRIX_IMP_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "multmatrix.h"
#include "../utils/socket.h"
#include "../utils/serializacion.h"
#include "../utils/tipos.h"

class MultmatrixImp
{
private:
    multMatrix *multmatrix = nullptr;

public:
    bool conexion_cerrada = false;

    void recibe_operacion(int client_id)
    {
        // Definir paquetes
        std::vector<unsigned char> packet_in, packet_out;

        // Recibir paquete y desempaquetar operacion
        recvMSG(client_id, packet_in);

        switch (unpack<e_operacion_multmatrix>(packet_in))
        {
            case MM_CONSTRUCTOR:
            {
                if (this->multmatrix == nullptr)
                {
                    this->multmatrix = new multMatrix();
                    pack<e_resultado_multmatrix>(packet_out, MM_OK);

                    // Mostar mensaje
                    std::cout << "MultmatrixImp: Objeto inicializado correctamente. Continuando..." << std::endl;
                }
                else pack<e_resultado_multmatrix>(packet_out, MM_NOCONSTRUCTOR);
            }
                break;

            case MM_DESTRUCTOR:
            {
                if (this->multmatrix != nullptr)
                {
                    delete this->multmatrix;
                    this->multmatrix = nullptr;
                    this->conexion_cerrada = true;
                    pack<e_resultado_multmatrix>(packet_out, MM_OK);

                    // Mostar mensaje
                    std::cout << "MultmatrixImp: Objeto eliminado correctamente. Cerrando conexion..." << std::endl;
                }
                else pack<e_resultado_multmatrix>(packet_out, MM_NODESTRUCTOR);

            }
                break;

            case MM_CREATERANDMATRIX:
            {
                if (this->multmatrix == nullptr) pack<e_resultado_multmatrix>(packet_out, MM_NOCREATERANDMATRIX);
                else
                {
                    pack<e_resultado_multmatrix>(packet_out, MM_OK);

                    // Empaquetar matrix
                    matrix_t *matrix = this->multmatrix->createRandMatrix(unpack<int>(packet_in), unpack<int>(packet_in));
                    serializar_matrix(packet_out, matrix);

                    // Liberar memoria
                    delete matrix;

                    // Mostar mensaje
                    std::cout << "MultmatrixImp: Matriz aleatoria creada correctamente. Continuando..." << std::endl;
                }
            }
                break;

            case MM_CREATEIDENTITY:
            {
                if (this->multmatrix == nullptr) pack<e_resultado_multmatrix>(packet_out, MM_NOCREATEIDENTITY);
                else
                {
                    pack<e_resultado_multmatrix>(packet_out, MM_OK);

                    // Empaquetar matrix
                    matrix_t *matrix = this->multmatrix->createIdentity(unpack<int>(packet_in), unpack<int>(packet_in));
                    serializar_matrix(packet_out, matrix);

                    // Liberar memoria
                    delete matrix;

                    // Mostar mensaje
                    std::cout << "MultmatrixImp: Matriz identidad creada correctamente. Continuando..." << std::endl;
                }
            }
                break;

            case MM_MULTMATRIX:
            {
                if (this->multmatrix == nullptr) pack<e_resultado_multmatrix>(packet_out, MM_NOMULTMATRIX);
                else
                {
                    // Desempaquetar matrices
                    matrix_t *m1 = new matrix_t, *m2 = new matrix_t;
                    deserializar_matrix(packet_in, m1);
                    deserializar_matrix(packet_in, m2);

                    // Multiplicar matrices
                    matrix_t *m3 = this->multmatrix->multMatrices(m1, m2);

                    // Empaquetar matrix
                    pack<e_resultado_multmatrix>(packet_out, MM_OK);
                    serializar_matrix(packet_out, m3);

                    // Liberar memoria
                    // Liberar .data
                    delete m1->data;
                    delete m1;
                    delete m2->data;
                    delete m2;
                    delete m3->data;
                    delete m3;

                    // Mostar mensaje
                    std::cout << "MultmatrixImp: Matrices multiplicadas correctamente. Continuando..." << std::endl;
                }
            }
                break;

            case MM_WRITEMATRIX:
            {
                if (this->multmatrix == nullptr) pack<e_resultado_multmatrix>(packet_out, MM_NOWRITEMATRIX);
                else
                {
                    int size = unpack<int>(packet_in);
                    char *file = new char[size];

                    unpackv(packet_in, file, size);

                    matrix_t *matrix = new matrix_t;
                    deserializar_matrix(packet_in, matrix);

                    this->multmatrix->writeMatrix(matrix, file);
                    pack<e_resultado_multmatrix>(packet_out, MM_OK);

                    // Liberar memoria
                    delete matrix;
                    delete[] file;

                    // Mostar mensaje
                    std::cout << "MultmatrixImp: Matriz escrita correctamente. Continuando..." << std::endl;
                }
            }
                break;

            case MM_READMATRIX:
            {
                if (this->multmatrix == nullptr) pack<e_resultado_multmatrix>(packet_out, MM_NOREADMATRIX);
                else
                {
                    int size = unpack<int>(packet_in);
                    char *file = new char[size];

                    unpackv(packet_in, file, size);

                    matrix_t *matrix = this->multmatrix->readMatrix(file);

                    if (matrix != nullptr)
                    {
                        pack<e_resultado_multmatrix>(packet_out, MM_OK);
                        serializar_matrix(packet_out, matrix);

                        // Mostar mensaje
                        std::cout << "MultmatrixImp: Matriz leida correctamente. Continuando..." << std::endl;
                    }
                    else pack<e_resultado_multmatrix>(packet_out, MM_INVALIDMATRIX);

                    // Liberar memoria
                    delete matrix;
                    delete[] file;
                }
            }
                break;

            default:
                std::cout << "MultmatrixImp: Operacion invalida. Ignorando..." << std::endl;
                pack<e_resultado_multmatrix>(packet_out, MM_ERROR);
                break;
        }

        // Enviar paquete
        sendMSG(client_id, packet_out);
    }
};

#endif //MULTMATRIX_MULTMATRIX_IMP_H
