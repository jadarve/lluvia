/**
 * \file test_Base64.cpp
 * \brief test base 64 encoding and decoding.
 * \copyright 2017, Juan David Adarve. See AUTHORS for more details
 * \license Apache 2.0, see LICENSE for more details
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "lluvia/core.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

void runTest(const std::vector<uint8_t>& bytes, const std::string& encoded)
{

    REQUIRE(encoded == ll::toBase64(bytes.data(), bytes.size() * sizeof(uint8_t)));

    auto decoded = ll::fromBase64(encoded);
    for (auto i = 0u; i < bytes.size(); ++i) {
        REQUIRE(bytes[i] == decoded[i]);
    }
}

/**
The following test data was generated using Python Numpy base64 encoder

\code{.py}

import base64
import numpy as np

np.random.seed(0)

for N in range(1, 25):
    for _ in range(5):
        bytes     = np.random.randint(0, 255, N, dtype=np.uint8)
        bytesText = ','.join([str(p) for p in bytes])
        encoded = base64.b64encode(bytes)
        print('runTest({{{0}}}, "{1}");'.format(bytesText, encoded))

\endcode
*/
TEST_CASE("encoding_decoding", "test_Base64")
{

    runTest({172}, "rA==");
    runTest({47}, "Lw==");
    runTest({117}, "dQ==");
    runTest({192}, "wA==");
    runTest({67}, "Qw==");
    runTest({251, 82}, "+1I=");
    runTest({195, 118}, "w3Y=");
    runTest({103, 125}, "Z30=");
    runTest({9, 164}, "CaQ=");
    runTest({211, 222}, "094=");
    runTest({21, 81, 89}, "FVFZ");
    runTest({242, 214, 102}, "8tZm");
    runTest({36, 183, 5}, "JLcF");
    runTest({87, 58, 43}, "Vzor");
    runTest({70, 60, 75}, "RjxL");
    runTest({216, 189, 132, 14}, "2L2EDg==");
    runTest({88, 154, 178, 246}, "WJqy9g==");
    runTest({140, 205, 204, 69}, "jM3MRQ==");
    runTest({58, 57, 41, 98}, "OjkpYg==");
    runTest({193, 66, 72, 122}, "wUJIeg==");
    runTest({230, 125, 174, 202, 39}, "5n2uyic=");
    runTest({87, 168, 101, 135, 174}, "V6hlh64=");
    runTest({88, 94, 107, 145, 81}, "WF5rkVE=");
    runTest({165, 230, 243, 236, 25}, "pebz7Bk=");
    runTest({77, 107, 47, 18, 72}, "TWsvEkg=");
    runTest({9, 27, 78, 22, 148, 151}, "CRtOFpSX");
    runTest({115, 8, 45, 5, 208, 19}, "cwgtBdAT");
    runTest({243, 146, 38, 213, 197, 30}, "85Im1cUe");
    runTest({254, 71, 53, 199, 79, 7}, "/kc1x08H");
    runTest({175, 29, 185, 222, 192, 9}, "rx253sAJ");
    runTest({82, 187, 134, 250, 99, 96, 62}, "UruG+mNgPg==");
    runTest({216, 167, 149, 204, 177, 124, 8}, "2KeVzLF8CA==");
    runTest({243, 130, 35, 118, 29, 3, 62}, "84Ijdh0DPg==");
    runTest({147, 194, 208, 199, 147, 12, 203}, "k8LQx5MMyw==");
    runTest({142, 59, 71, 30, 167, 97, 123}, "jjtHHqdhew==");
    runTest({32, 221, 209, 163, 193, 63, 148, 9}, "IN3Ro8E/lAk=");
    runTest({185, 74, 145, 137, 127, 210, 213, 241}, "uUqRiX/S1fE=");
    runTest({32, 162, 52, 194, 31, 218, 151, 133}, "IKI0wh/al4U=");
    runTest({202, 194, 28, 27, 244, 72, 39, 106}, "ysIcG/RIJ2o=");
    runTest({151, 224, 61, 121, 163, 234, 185, 67}, "l+A9eaPquUM=");
    runTest({254, 121, 179, 47, 203, 45, 52, 198, 114}, "/nmzL8stNMZy");
    runTest({183, 68, 198, 116, 28, 216, 111, 55, 34}, "t0TGdBzYbzci");
    runTest({128, 168, 157, 34, 128, 104, 207, 4, 164}, "gKidIoBozwSk");
    runTest({53, 92, 29, 158, 133, 23, 81, 38, 38}, "NVwdnoUXUSYm");
    runTest({232, 13, 234, 56, 244, 98, 239, 157, 17}, "6A3qOPRi750R");
    runTest({79, 121, 153, 241, 132, 177, 16, 231, 105, 198}, "T3mZ8YSxEOdpxg==");
    runTest({42, 236, 47, 115, 186, 181, 8, 92, 31, 186}, "Kuwvc7q1CFwfug==");
    runTest({120, 83, 225, 111, 1, 81, 0, 231, 65, 245}, "eFPhbwFRAOdB9Q==");
    runTest({231, 111, 106, 25, 169, 239, 106, 15, 57, 104}, "529qGanvag85aA==");
    runTest({35, 57, 177, 170, 102, 47, 52, 167, 119, 236}, "IzmxqmYvNKd37A==");
    runTest({11, 187, 192, 43, 174, 161, 219, 53, 82, 220, 175}, "C7vAK66h2zVS3K8=");
    runTest({91, 80, 1, 33, 128, 247, 44, 192, 142, 233, 191}, "W1ABIYD3LMCO6b8=");
    runTest({99, 202, 154, 155, 53, 38, 28, 93, 140, 75, 54}, "Y8qamzUmHF2MSzY=");
    runTest({121, 106, 248, 145, 170, 63, 214, 9, 84, 48, 72}, "eWr4kao/1glUMEg=");
    runTest({203, 200, 95, 162, 68, 17, 6, 253, 6, 179, 125}, "y8hfokQRBv0Gs30=");
    runTest({196, 155, 31, 26, 47, 68, 29, 167, 127, 242, 120, 53}, "xJsfGi9EHad/8ng1");
    runTest({244, 55, 147, 162, 131, 148, 75, 41, 204, 243, 203, 254}, "9DeTooOUSynM88v+");
    runTest({100, 27, 50, 167, 180, 36, 244, 148, 232, 183, 215, 64}, "ZBsyp7Qk9JTot9dA");
    runTest({78, 15, 20, 106, 143, 36, 96, 119, 148, 198, 133, 121}, "Tg8Uao8kYHeUxoV5");
    runTest({227, 172, 146, 62, 186, 91, 158, 159, 23, 59, 178, 40}, "46ySPrpbnp8XO7Io");
    runTest({207, 170, 135, 86, 141, 139, 65, 28, 117, 145, 188, 172, 85}, "z6qHVo2LQRx1kbysVQ==");
    runTest({48, 34, 52, 81, 49, 245, 95, 35, 69, 166, 65, 199, 169}, "MCI0UTH1XyNFpkHHqQ==");
    runTest({163, 22, 23, 243, 192, 197, 100, 94, 95, 92, 155, 169, 197}, "oxYX88DFZF5fXJupxQ==");
    runTest({94, 110, 121, 3, 0, 161, 219, 24, 113, 215, 114, 159, 178}, "Xm55AwCh2xhx13Kfsg==");
    runTest({36, 245, 116, 172, 162, 231, 153, 24, 48, 99, 209, 248, 93}, "JPV0rKLnmRgwY9H4XQ==");
    runTest({131, 73, 209, 224, 98, 134, 249, 119, 42, 190, 118, 130, 205, 3}, "g0nR4GKG+XcqvnaCzQM=");
    runTest({112, 81, 67, 14, 231, 39, 215, 154, 149, 43, 127, 115, 201, 96}, "cFFDDucn15qVK39zyWA=");
    runTest({127, 233, 29, 5, 0, 54, 8, 10, 138, 247, 19, 113, 114, 9}, "f+kdBQA2CAqK9xNxcgk=");
    runTest({43, 50, 198, 250, 186, 51, 197, 30, 127, 141, 4, 92, 23, 216}, "KzLG+rozxR5/jQRcF9g=");
    runTest({187, 214, 27, 123, 130, 240, 100, 30, 121, 25, 76, 176, 98, 88}, "u9Ybe4LwZB55GUywYlg=");
    runTest({62, 222, 102, 225, 163, 35, 13, 106, 222, 122, 17, 235, 123, 248, 107}, "Pt5m4aMjDWreehHre/hr");
    runTest({195, 167, 129, 55, 82, 218, 69, 177, 174, 55, 176, 144, 227, 202, 12}, "w6eBN1LaRbGuN7CQ48oM");
    runTest({148, 92, 119, 221, 209, 144, 240, 67, 50, 202, 75, 130, 155, 149, 243}, "lFx33dGQ8EMyykuCm5Xz");
    runTest({14, 91, 174, 234, 41, 124, 12, 24, 58, 252, 208, 235, 193, 58, 113}, "Dluu6il8DBg6/NDrwTpx");
    runTest({36, 220, 70, 21, 10, 91, 230, 237, 86, 144, 24, 71, 43, 188, 141}, "JNxGFQpb5u1WkBhHK7yN");
    runTest({104, 51, 101, 2, 11, 104, 219, 170, 2, 187, 163, 215, 51, 129, 189, 33}, "aDNlAgto26oCu6PXM4G9IQ==");
    runTest({80, 52, 173, 165, 32, 56, 97, 183, 182, 20, 101, 215, 128, 132, 22, 74}, "UDStpSA4Ybe2FGXXgIQWSg==");
    runTest({38, 91, 197, 67, 19, 161, 229, 46, 174, 148, 215, 101, 42, 182, 37, 150}, "JlvFQxOh5S6ulNdlKrYllg==");
    runTest({115, 181, 133, 141, 184, 196, 37, 5, 188, 137, 57, 42, 232, 105, 53, 212}, "c7WFjbjEJQW8iTkq6Gk11A==");
    runTest({77, 190, 171, 94, 30, 185, 51, 1, 24, 53, 125, 37, 125, 98, 133, 173}, "Tb6rXh65MwEYNX0lfWKFrQ==");
    runTest({2, 131, 210, 145, 3, 62, 31, 69, 94, 32, 40, 180, 226, 172, 53, 188, 107}, "AoPSkQM+H0VeICi04qw1vGs=");
    runTest({13, 253, 81, 246, 112, 247, 235, 110, 40, 73, 174, 63, 72, 53, 144, 193, 19}, "Df1R9nD3624oSa4/SDWQwRM=");
    runTest({95, 178, 102, 101, 72, 15, 144, 151, 154, 197, 98, 229, 194, 25, 127, 146, 248}, "X7JmZUgPkJeaxWLlwhl/kvg=");
    runTest({180, 224, 27, 57, 67, 233, 60, 228, 236, 91, 231, 243, 61, 31, 24, 174, 14}, "tOAbOUPpPOTsW+fzPR8Yrg4=");
    runTest({96, 159, 254, 114, 4, 61, 174, 216, 195, 159, 131, 250, 237, 18, 17, 179, 139}, "YJ/+cgQ9rtjDn4P67RIRs4s=");
    runTest({252, 211, 36, 76, 86, 170, 91, 196, 205, 13, 85, 208, 121, 11, 109, 105, 109, 102}, "/NMkTFaqW8TNDVXQeQttaW1m");
    runTest({75, 147, 233, 172, 184, 250, 143, 225, 16, 166, 242, 63, 152, 76, 206, 148, 157, 19}, "S5PprLj6j+EQpvI/mEzOlJ0T");
    runTest({149, 104, 185, 225, 110, 132, 37, 247, 25, 192, 73, 177, 208, 165, 165, 150, 188, 67}, "lWi54W6EJfcZwEmx0KWllrxD");
    runTest({121, 7, 224, 168, 118, 203, 86, 128, 117, 52, 128, 136, 189, 229, 193, 244, 83, 54}, "eQfgqHbLVoB1NICIveXB9FM2");
    runTest({161, 138, 220, 164, 104, 39, 22, 101, 160, 195, 129, 108, 228, 60, 106, 158, 251, 149}, "oYrcpGgnFmWgw4Fs5DxqnvuV");
    runTest({251, 234, 144, 121, 121, 216, 233, 4, 70, 149, 90, 120, 213, 1, 52, 77, 31, 169, 80}, "++qQeXnY6QRGlVp41QE0TR+pUA==");
    runTest({13, 34, 1, 169, 71, 199, 185, 73, 184, 134, 66, 74, 152, 148, 42, 98, 79, 66, 54}, "DSIBqUfHuUm4hkJKmJQqYk9CNg==");
    runTest({41, 152, 201, 191, 18, 201, 195, 109, 40, 62, 226, 224, 182, 109, 174, 34, 207, 64, 85}, "KZjJvxLJw20oPuLgtm2uIs9AVQ==");
    runTest({11, 59, 92, 76, 166, 208, 165, 23, 111, 56, 233, 145, 93, 205, 162, 90, 249, 111, 67}, "CztcTKbQpRdvOOmRXc2iWvlvQw==");
    runTest({129, 212, 67, 141, 223, 250, 6, 147, 118, 167, 155, 8, 44, 43, 56, 167, 216, 224, 13}, "gdRDjd/6BpN2p5sILCs4p9jgDQ==");
    runTest({125, 61, 240, 166, 24, 224, 44, 82, 67, 112, 113, 110, 210, 171, 164, 56, 239, 19, 132, 229}, "fT3wphjgLFJDcHFu0qukOO8ThOU=");
    runTest({3, 223, 41, 36, 234, 136, 24, 94, 204, 6, 230, 24, 230, 215, 148, 111, 35, 49, 234, 251}, "A98pJOqIGF7MBuYY5teUbyMx6vs=");
    runTest({214, 22, 85, 228, 254, 179, 165, 66, 189, 186, 98, 206, 197, 78, 122, 137, 215, 10, 50, 180}, "1hZV5P6zpUK9umLOxU56idcKMrQ=");
    runTest({43, 74, 171, 114, 32, 120, 168, 25, 11, 92, 125, 25, 104, 54, 99, 235, 212, 24, 49, 90}, "K0qrciB4qBkLXH0ZaDZj69QYMVo=");
    runTest({138, 132, 216, 182, 182, 182, 32, 120, 235, 111, 180, 165, 245, 84, 215, 125, 62, 66, 38, 156}, "ioTYtra2IHjrb7Sl9VTXfT5CJpw=");
    runTest({111, 130, 61, 222, 232, 125, 157, 9, 2, 35, 153, 41, 27, 246, 32, 130, 211, 79, 149, 157, 217}, "b4I93uh9nQkCI5kpG/YggtNPlZ3Z");
    runTest({151, 170, 178, 31, 53, 227, 111, 199, 51, 17, 23, 217, 174, 68, 108, 221, 148, 116, 172, 206, 181}, "l6qyHzXjb8czERfZrkRs3ZR0rM61");
    runTest({29, 150, 176, 145, 67, 62, 213, 35, 35, 42, 61, 104, 39, 67, 129, 8, 137, 237, 180, 17, 73}, "HZawkUM+1SMjKj1oJ0OBCInttBFJ");
    runTest({41, 177, 138, 178, 151, 251, 143, 95, 131, 95, 27, 116, 46, 15, 138, 107, 218, 162, 216, 184, 178}, "KbGKspf7j1+DXxt0Lg+Ka9qi2Liy");
    runTest({108, 59, 203, 221, 3, 39, 145, 93, 31, 199, 187, 249, 9, 187, 65, 4, 138, 237, 21, 219, 27}, "bDvL3QMnkV0fx7v5CbtBBIrtFdsb");
    runTest({173, 177, 2, 199, 64, 209, 195, 167, 133, 39, 92, 61, 20, 178, 241, 85, 169, 224, 186, 97, 243, 191}, "rbECx0DRw6eFJ1w9FLLxVangumHzvw==");
    runTest({44, 236, 239, 43, 34, 43, 227, 86, 162, 167, 98, 133, 88, 20, 83, 125, 33, 24, 233, 13, 133, 186}, "LOzvKyIr41aip2KFWBRTfSEY6Q2Fug==");
    runTest({232, 248, 50, 51, 18, 243, 45, 36, 216, 189, 4, 0, 199, 197, 43, 203, 197, 47, 203, 34, 185, 164}, "6PgyMxLzLSTYvQQAx8Ury8UvyyK5pA==");
    runTest({197, 32, 83, 57, 126, 3, 228, 223, 181, 247, 104, 88, 254, 212, 177, 241, 80, 188, 150, 237, 190, 117}, "xSBTOX4D5N+192hY/tSx8VC8lu2+dQ==");
    runTest({136, 128, 84, 180, 189, 104, 190, 152, 129, 152, 38, 8, 209, 232, 156, 200, 112, 101, 41, 42, 35, 185}, "iIBUtL1ovpiBmCYI0eicyHBlKSojuQ==");
    runTest({120, 53, 25, 159, 91, 13, 229, 12, 168, 64, 197, 147, 116, 23, 248, 178, 36, 139, 230, 60, 176, 176, 13}, "eDUZn1sN5QyoQMWTdBf4siSL5jywsA0=");
    runTest({25, 166, 40, 239, 67, 81, 107, 68, 103, 223, 44, 157, 252, 225, 216, 173, 35, 59, 31, 137, 114, 140, 65}, "GaYo70NRa0Rn3yyd/OHYrSM7H4lyjEE=");
    runTest({30, 87, 4, 151, 29, 84, 55, 192, 241, 70, 233, 186, 33, 205, 232, 246, 146, 160, 219, 79, 17, 213, 226}, "HlcElx1UN8DxRum6Ic3o9pKg208R1eI=");
    runTest({221, 208, 241, 101, 84, 57, 95, 54, 253, 81, 184, 53, 2, 10, 242, 56, 69, 88, 170, 47, 101, 245, 55}, "3dDxZVQ5Xzb9Ubg1AgryOEVYqi9l9Tc=");
    runTest({140, 99, 194, 241, 44, 146, 207, 145, 117, 51, 83, 189, 253, 106, 189, 115, 66, 181, 142, 125, 111, 110, 97}, "jGPC8SySz5F1M1O9/Wq9c0K1jn1vbmE=");
    runTest({91, 216, 55, 58, 85, 45, 56, 174, 167, 129, 29, 65, 39, 238, 213, 21, 203, 218, 14, 150, 110, 113, 14, 158}, "W9g3OlUtOK6ngR1BJ+7VFcvaDpZucQ6e");
    runTest({145, 238, 226, 124, 198, 218, 209, 79, 199, 131, 137, 225, 18, 145, 67, 178, 92, 163, 245, 249, 43, 88, 180, 96}, "ke7ifMba0U/Hg4nhEpFDslyj9fkrWLRg");
    runTest({83, 212, 30, 158, 177, 129, 250, 45, 41, 51, 225, 138, 93, 88, 81, 6, 174, 244, 199, 218, 149, 69, 55, 17}, "U9QenrGB+i0pM+GKXVhRBq70x9qVRTcR");
    runTest({201, 240, 107, 190, 89, 175, 236, 173, 242, 73, 133, 122, 224, 121, 37, 116, 219, 55, 85, 173, 73, 65, 93, 137}, "yfBrvlmv7K3ySYV64HkldNs3Va1JQV2J");
    runTest({28, 78, 103, 155, 235, 63, 140, 229, 209, 96, 246, 182, 105, 218, 134, 253, 186, 247, 48, 120, 128, 143, 134, 55}, "HE5nm+s/jOXRYPa2adqG/br3MHiAj4Y3");
}
