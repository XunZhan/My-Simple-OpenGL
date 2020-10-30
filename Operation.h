#pragma once
#include	"gz.h"

struct Vec3 {
	float v[3];
	Vec3(float xx = 0.0, float yy = 0.0, float zz = 0.0)
	{
		v[0] = xx;
		v[1] = yy;
		v[2] = zz;
	}

	Vec3(float* a)
	{
		v[0] = a[0];
		v[1] = a[1];
		v[2] = a[2];
	}

	Vec3 operator+ (const Vec3 &b) const {
		return Vec3(v[0] + b.v[0], v[1] + b.v[1], v[2] + b.v[2]);
	}

	Vec3& operator+= (const Vec3 &b) {
		this->v[0] += b.v[0];
		this->v[1] += b.v[1];
		this->v[2] += b.v[2];
		return *this;
	}

	Vec3 operator- (const Vec3 &b) const {
		return Vec3(v[0] - b.v[0], v[1] - b.v[1], v[2] - b.v[2]);
	}

	Vec3& operator-= (const Vec3 &b) {
		this->v[0] -= b.v[0];
		this->v[1] -= b.v[1];
		this->v[2] -= b.v[2];
		return *this;
	}

	Vec3 operator% (const Vec3 &b) const {
		return Vec3(v[0] * b.v[0], v[1] * b.v[1], v[2] * b.v[2]);
	}

	Vec3 operator* (float b) const {
		return Vec3(v[0] * b, v[1] * b, v[2] * b);
	}

	Vec3& operator*= (const Vec3 &b) {
		this->v[0] *= b.v[0];
		this->v[1] *= b.v[1];
		this->v[2] *= b.v[2];
		return *this;
	}

	float dot(const Vec3 &b) const {
		return v[0] * b.v[0] + v[1] * b.v[1] + v[2] * b.v[2];
	}

	Vec3 cross(const Vec3 &b) const {
		return Vec3(v[1] * b.v[2] - v[2] * b.v[1],
			v[2] * b.v[0] - v[0] * b.v[2],
			v[0] * b.v[1] - v[1] * b.v[0]);
	}

	Vec3& normlize() {
		return *this = *this * (1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
	}
};

struct Mat4 {
	GzMatrix m;

	// init identity matrix
	Mat4(bool identity)
	{
		if (identity)
		{
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
				{
					m[i][j] = (i == j) ? 1.0 : 0.0;
				}
		}
		else
		{
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
				{
					m[i][j] = 0.0;
				}
		}

	}

	Mat4(GzMatrix b)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m[i][j] = b[i][j];
	}

	Mat4 operator+ (const Mat4 &b) const {
		Mat4 ans = Mat4(false);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				ans.m[i][j] = m[i][j] + b.m[i][j];
		return ans;
	}

	Mat4 operator- (const Mat4 &b) const {
		Mat4 ans = Mat4(false);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				ans.m[i][j] = m[i][j] - b.m[i][j];
		return ans;
	}

	Mat4 operator* (const Mat4 &b) const {
		Mat4 ans = Mat4(false);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					ans.m[i][j] += m[i][k] * b.m[k][j];
		return ans;
	}

	Mat4 operator* (const Vec3 &b) const {
		Mat4 ans = Mat4(false);
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 3; k++)
				ans.m[i][0] += m[i][k] * b.v[k];
			ans.m[i][0] += m[i][3] * 1.0;
		}

		return ans;
	}
};

