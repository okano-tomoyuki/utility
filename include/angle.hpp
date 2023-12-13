#include <cmath>

class Angle
{
private:
    double rad_;

    explicit Angle(const double& rad) noexcept
     : rad_(rad)
    {}

    static double adjust_pi(const double& rad) noexcept
    {
        double result = rad;
        while(result > M_PI)
            result -= 2 * M_PI;
        while(result < - M_PI)
            result += 2 * M_PI;
        return result;
    }

public:

    static Angle make_deg(const double& deg) noexcept
    {
        return Angle(adjust_pi(deg * M_PI / 180.));
    }

    static Angle make_rad(const double& rad) noexcept
    {
        return Angle(adjust_pi(rad));
    }

    double deg() const noexcept
    {
        return rad_ * 180. / M_PI;
    }

    double rad() const noexcept
    {
        return rad_;
    }

    void operator=(const Angle& other)
    {
        rad_ = other.rad();
    }

    bool operator==(const Angle& other)
    {
        return (rad_ == other.rad());
    }

    bool operator!=(const Angle& other)
    {
        return !(*this==other);
    }

};

Angle operator+(const Angle& lhs, const Angle& rhs)
{
    return Angle::make_rad(lhs.rad()+rhs.rad());
}

Angle operator-(const Angle& lhs, const Angle& rhs)
{
    return Angle::make_rad(lhs.rad()-rhs.rad());
}
