#include "slowbackend.h"
#include <QTimer>

SlowBackend::SlowBackend(QObject *parent) : QObject(parent)
{

}

double SlowBackend::value() const
{
    return m_value;
}

void SlowBackend::setValue(double value)
{
    QTimer::singleShot(500, this, [=] { setDirectValue(value); });
}

void SlowBackend::setDirectValue(double directValue)
{
    if (m_value == directValue)
        return;

    m_value = directValue;
    emit valueChanged();
}
