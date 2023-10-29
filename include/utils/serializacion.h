//
// Created by adria on 29/10/2023.
//

#pragma once
#ifndef MULTMATRIX_SERIALIZACION_H
#define MULTMATRIX_SERIALIZACION_H

#include <vector>


template<typename T>
inline void pack(std::vector<unsigned char> &packet, T data)
{
    auto *ptr = (unsigned char *) &data;
    int dsize = sizeof(T), init = packet.size();

    packet.resize(init + dsize);
    std::copy(ptr, ptr + dsize, packet.begin() + init);
}

template<typename T>
inline T unpack(std::vector<unsigned char> &packet)
{
    T *ptr = (T *) packet.data();
    T data = ptr[0];
    int dsize = sizeof(T), init = packet.size();

    for (int i = dsize; i < packet.size(); i++) packet[i - dsize] = packet[i];
    packet.resize(init - dsize);

    return data;
}

template<typename T>
inline void packv(std::vector<unsigned char> &packet, T *arr, int size)
{
    for (int i = 0; i < size; i++) pack(packet, arr[i]);
}

template<typename T>
inline void unpackv(std::vector<unsigned char> &packet, T *arr, int size)
{
    for (int i = 0; i < size; i++) arr[i] = unpack<T>(packet);
}

#endif //MULTMATRIX_SERIALIZACION_H
