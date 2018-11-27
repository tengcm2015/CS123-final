#include "BGRA.h"

BGRA BGRA::operator+(const BGRA &that) const {
    return BGRA(this->r + that.r, this->g + that.g, this->b + that.b, this->a + that.a);
}

BGRA BGRA::operator-(const BGRA &that) const {
    return BGRA(this->r - that.r, this->g - that.g, this->b - that.b, this->a - that.a);
}

BGRA BGRA::operator*(const BGRA &that) const {
    return BGRA(this->r * that.r, this->g * that.g, this->b * that.b, this->a * that.a);
}

BGRA BGRA::operator/(const BGRA &that) const {
    return BGRA(this->r / that.r, this->g / that.g, this->b / that.b, this->a / that.a);
}

bool operator==(const BGRA &c1, const BGRA &c2) {
    return (c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a);
}

bool operator!=(const BGRA &c1, const BGRA &c2) {
    return !operator==(c1, c2);
}
