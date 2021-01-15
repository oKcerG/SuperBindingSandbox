#ifndef SLOWBACKEND_H
#define SLOWBACKEND_H

#include <QObject>

class SlowBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double directValue READ value WRITE setDirectValue NOTIFY valueChanged)


public:
    explicit SlowBackend(QObject *parent = nullptr);

    double value() const;
    void setValue(double value);
    void setDirectValue(double directValue);

signals:
    void valueChanged();

private:
    double m_value = 0;
};

#endif // SLOWBACKEND_H
