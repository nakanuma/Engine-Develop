#include "Transform.h"

Matrix Transform::MakeAffineMatrix()
{
    Matrix result = Matrix::Identity();

    // SRTの順番で行列を生成してかける
    result *= Matrix::Scaling({ scale.x, scale.y, scale.z });
    result *= Matrix::RotationRollPitchYaw(rotate.z, rotate.x, rotate.y);
    result *= Matrix::Translation({ translate.x, translate.y, translate.z });

    return result;
}