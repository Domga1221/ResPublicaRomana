#pragma once 

#include <cassert>

#include <vector>
struct Hex {
    // Cube
    int q;
    int r;
    int s; // s = -q - r

    // Offset
    int col; // along x axis
    int row; // along z axis

    const static int EVEN = +1;
    const static int ODD  = -1;
    void OffsetToCube(int offset) {
        int q = col - (int)((row + offset * (row & 1)) / 2);
        int r = row;
        int s = -q - r;
        this->q = q;
        this->r = r;
        this->s = s;
    }

    Hex() = default;
    Hex(int q_, int r_, int s_) : q(q_), r(r_), s(s_) {
        assert(q + r + s == 0);
    }
    bool operator==(const Hex& other) const {
        return this->q == other.q && this->r == other.r && this->s == other.s;
    }
    bool operator!=(const Hex& other) const {
        return !(*this == other);
    }

};

Hex Hex_Add(Hex a, Hex b ) {
    return Hex(a.q + b.q, a.r + b.r, a.s + b.s);
}

Hex Hex_Subtract(Hex a, Hex b) {
    return Hex(a.q - b.q, a.r - b.r, a.s - b.s);
}

Hex Hex_Multiply(Hex a, int k) {
    return Hex(a.q * k, a.r * k, a.s * k);
}

int Hex_Length(Hex hex) {
    return int((abs(hex.q) + abs (hex.r) + abs(hex.s)) / 2);
}

int Hex_Distance(Hex a, Hex b) {
    return Hex_Length(Hex_Subtract(a, b));
}

// To make directions outside the range 0..5 work, use hex_directions[(6 + (direction % 6)) % 6]
const std::vector<Hex> Hex_Directions = {
    Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1), 
    Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
};

/* 0 to 5 */
Hex Hex_Direction(int direction) {
    assert(0 <= direction && direction < 6);
    return Hex_Directions[direction];
}

Hex Hex_Neighbor(Hex hex, int direction) {
    return Hex_Add(hex, Hex_Direction(direction));
}


int Hex_Hash(Hex a) {
    std::hash<int> intHash;
    int hq = intHash(a.q);
    int hr = intHash(a.r);
    return hq ^ (hr + (int)0x9e3779b + (hq << 6) + (hq >> 2));
}

#include <string>
std::string Hex_ToString(Hex hex) {
    return std::string("(" + std::to_string(hex.q) + ", " + std::to_string(hex.r) + ", " + std::to_string(hex.s) + ")");
}


// 
//  glm is column major 
//  mat[col][row]
//  
//  MATRIX 2x2 layout
//          col     col 
//          0       1   
//  row 0   [0][0]  [1][0]
//  row 1   [0][1]  [1][1]
#include <glm/glm.hpp>
glm::vec2 Hex_ToPoint(Hex hex, float size) {
    glm::mat2 mat = glm::mat2(1.0f);
    mat[0][0] = glm::sqrt(3.0f);
    mat[1][0] = glm::sqrt(3.0f) / 2.0f;
    mat[0][1] = 0.0f;
    mat[1][1] = 3.0f / 2.0f;

    glm::vec2 vec = glm::vec2(hex.q, hex.r);
    glm::vec2 result = mat * vec;
    return result;
}