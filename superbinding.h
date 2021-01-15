#ifndef SUPERBINDING_H
#define SUPERBINDING_H

#include <QObject>
#include <QQmlProperty>
#include <QQmlParserStatus>
#include <QQmlPropertyValueSource>
#include <QTimer>

class SuperBinding : public QObject, public QQmlParserStatus, public QQmlPropertyValueSource
{
    Q_OBJECT
    Q_PROPERTY(QQmlProperty source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QQmlProperty target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(bool modifying READ modifying WRITE setModifying NOTIFY modifyingChanged)
    Q_PROPERTY(DelayMode delayMode READ delayMode WRITE setDelayMode NOTIFY delayModeChanged)
    Q_PROPERTY(int delay READ delay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(bool pendingWrite READ pendingWrite NOTIFY pendingWriteChanged)
    Q_PROPERTY(bool pendingUpdate READ pendingUpdate NOTIFY pendingUpdateChanged)
    Q_PROPERTY(bool pending READ pending NOTIFY pendingChanged)

public:
    enum DelayMode {
        DelayNone,
        DelayThrottle,
        DelayDebounce,
        DelayWhileModifying
    };
    Q_ENUM(DelayMode)

    SuperBinding(QObject* parent = nullptr);

    QQmlProperty source() const;
    void setSource(const QQmlProperty& source);

    void setTarget(const QQmlProperty& target) override;
    QQmlProperty target() const;

    bool modifying() const;
    void setModifying(bool modifying);

    DelayMode delayMode() const;
    void setDelayMode(DelayMode delayMode);

    int delay() const;
    void setDelay(int delay);

    int timeout() const;
    void setTimeout(int timeout);

    bool pendingWrite() const;
    bool pendingUpdate() const;
    bool pending();

    Q_INVOKABLE QQmlProperty qtProperty(QObject* object, const QString& name);

    void classBegin() override;
    void componentComplete() override;

signals:
    void sourceChanged();
    void targetChanged();
    void modifyingChanged();
    void delayModeChanged();
    void delayChanged();
    void timeoutChanged();
    void pendingWriteChanged();
    void pendingUpdateChanged();
    void pendingChanged();

private slots:
    void onSourceValueChanged();
    void onTargetValueChanged();

private:
    void writeToSource();
    void writeToTarget();

    QQmlProperty m_source;
    QMetaObject::Connection m_sourceNotifyConnection;
    QQmlProperty m_target;
    QMetaObject::Connection m_targetNotifyConnection;
    bool m_modifying = false;
    DelayMode m_delayMode = DelayNone;
    int m_delay = 0;
    int m_timeout = 0;

    QTimer* m_delayTimer = new QTimer(this);
    QTimer* m_timeoutTimer = new QTimer(this);
};

#endif // SUPERBINDING_H
