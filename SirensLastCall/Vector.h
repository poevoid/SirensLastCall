#pragma once

struct Vector {
  float x, y;

  Vector operator=(const Vector& other) const {
    return {other.x, other.y};
  }
  void operator=(const Vector& other) {
    x = other.x;
    y = other.y;
  }
  Vector operator +(const Vector& other) const {
    return {x + other.x, y + other.y};
  }

  void operator +=(const Vector& other) {
    x += other.x;
    y += other.y;
  }

  Vector operator -(const Vector& other) const {
    return {x - other.x, y - other.y};
  }

  void operator -=(const Vector& other) {
    x -= other.x;
    y -= other.y;
  }

  Vector operator *(const float factor) const {
    return {x * factor, y * factor};
  }

  void operator *=(const float factor) {
    x *= factor;
    y *= factor;
  }
 
  Vector operator /(const float divisor) const {
    return {x / divisor, y / divisor};
  }

  void operator /=(const float divisor) {
    x /= divisor;
    y /= divisor;
  }

  // For distance comparisons, we do not need exact distance.
  // For optimisation purposes, we can use squared distance,
  // so we can avoid expensive sqrt call.
  float distanceSquared(const Vector& other) const {
    return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
  }

  float magnitudeSquared() const {
    return (x * x) + (y * y);
  }

  float mag() const {
    return sqrt(x*x +y*y);
  }  
  
  float norm() const {
    float m = Vector::mag();
    if (m>0){
      Vector::operator/=(m);
    }
  }
float setMag(float n) {
    Vector::norm();
    Vector::operator*=(n);
    
  }
  void limit(float max) const {
    if (Vector::mag() > max){
      Vector::norm();
      Vector::operator*=(max);
    }
  }

};