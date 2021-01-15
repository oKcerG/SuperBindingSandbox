#include "superbinding.h"
#include <QDebug>

SuperBinding::SuperBinding(QObject* parent) : QObject(parent)
{
    m_delayTimer->setInterval(1000);
    m_delayTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(0);
    m_timeoutTimer->setSingleShot(true);
    connect(m_delayTimer, &QTimer::timeout, this, &SuperBinding::writeToSource);
    connect(m_timeoutTimer, &QTimer::timeout, this, &SuperBinding::onSourceValueChanged);
    connect(m_delayTimer, &QTimer::timeout, this, &SuperBinding::pendingWriteChanged);
    connect(this, &SuperBinding::pendingWriteChanged, this, &SuperBinding::pendingChanged);
    connect(this, &SuperBinding::pendingUpdateChanged, this, &SuperBinding::pendingChanged);
}

QQmlProperty SuperBinding::source() const
{
    return m_source;
}

void SuperBinding::setSource(const QQmlProperty& source)
{
    if (m_source == source)
        return;

    m_source = source;

    disconnect(m_sourceNotifyConnection);

    const QMetaObject* mo = metaObject();
    QMetaMethod onSourceValueChangedMetaMethod = mo->method(mo->indexOfSlot("onSourceValueChanged()"));

    if (source.hasNotifySignal())
        m_sourceNotifyConnection = connect(source.object(), source.property().notifySignal(), this, onSourceValueChangedMetaMethod);
    else
        m_sourceNotifyConnection = {};

    emit sourceChanged();
    qDebug() << "source" << source.object() << source.name() << source.hasNotifySignal() << !!m_sourceNotifyConnection << source.read();
}

void SuperBinding::setTarget(const QQmlProperty& target)
{
    if (m_target == target)
        return;

    m_target = target;

    disconnect(m_targetNotifyConnection);

    const QMetaObject* mo = metaObject();
    QMetaMethod onTargetValueChangedMetaMethod = mo->method(mo->indexOfSlot("onTargetValueChanged()"));

    if (target.hasNotifySignal())
        m_targetNotifyConnection = connect(target.object(), target.property().notifySignal(), this, onTargetValueChangedMetaMethod);
    else
        m_targetNotifyConnection = {};

    emit targetChanged();
}

QQmlProperty SuperBinding::target() const
{
    return m_target;
}

bool SuperBinding::modifying() const
{
    return m_modifying;
}

void SuperBinding::setModifying(bool modifying)
{
    if (m_modifying == modifying)
        return;

    m_modifying = modifying;
    emit modifyingChanged();

    if (!modifying) {
        if (m_delayMode == DelayNone) {
            onSourceValueChanged();
        } else {
            m_delayTimer->stop();
            writeToSource();
        }
    }
}

SuperBinding::DelayMode SuperBinding::delayMode() const
{
    return m_delayMode;
}

void SuperBinding::setDelayMode(SuperBinding::DelayMode delayMode)
{
    if (m_delayMode == delayMode)
        return;

    m_delayMode = delayMode;
    emit delayModeChanged();
}

int SuperBinding::delay() const
{
    return m_delayTimer->interval();
}

void SuperBinding::setDelay(int delay)
{
    if (this->delay() == delay)
        return;

    m_delayTimer->setInterval(delay);
    emit delayChanged();
}

int SuperBinding::timeout() const
{
    return m_timeoutTimer->interval();
}

void SuperBinding::setTimeout(int timeout)
{
    if (this->timeout() == timeout)
        return;

    m_timeoutTimer->setInterval(timeout);
    emit timeoutChanged();
}

bool SuperBinding::pendingWrite() const
{
    return m_delayTimer->isActive();
}

bool SuperBinding::pendingUpdate() const
{
    return m_timeoutTimer->isActive();
}

bool SuperBinding::pending()
{
    return m_delayTimer->isActive() || m_timeoutTimer->isActive();
}

QQmlProperty SuperBinding::qtProperty(QObject* object, const QString& name)
{
    return QQmlProperty(object, name);
}

void SuperBinding::classBegin()
{
}

void SuperBinding::componentComplete()
{
    writeToTarget();
}

void SuperBinding::onSourceValueChanged()
{
    qDebug() << "onSourceValueChanged" << (m_source.isValid() && m_target.isValid()) << m_source.read() << "-->" << m_target.read();
    if (m_modifying)
        return;
    if (m_timeoutTimer->isActive()) {
        //if (m_source.read() == m_target.read())
            m_timeoutTimer->stop();
    } else {
        writeToTarget();
    }
}

void SuperBinding::onTargetValueChanged()
{
    qDebug() << "onTargetValueChanged" << (m_source.isValid() && m_target.isValid()) << m_source.read() << "<--" << m_target.read();

    if (m_source.isValid() && m_target.isValid()) {
        if (m_delayMode == DelayWhileModifying)
            return;

        if (m_target.read() != m_source.read()) {
            if (m_delayMode == DelayNone) {
                writeToSource();
            } else if (m_delayMode == DelayDebounce
                       || (m_delayMode == DelayThrottle && !m_delayTimer->isActive())) {
                m_delayTimer->start();
            }
        }
    }
}

void SuperBinding::writeToSource()
{
    if (m_source.isValid() && m_target.isValid()) {
        QVariant targetValue = m_target.read();
        m_source.write(targetValue);
        if (timeout()) {
            m_timeoutTimer->start();
            return;
        }
        if (m_modifying)
            return;
        QVariant sourceValue = m_source.read();
        qDebug() << "write ?" << sourceValue << targetValue;
        if (sourceValue != targetValue)
            m_target.write(sourceValue);
    }
}

void SuperBinding::writeToTarget()
{
    if (m_source.isValid() && m_target.isValid() && !m_modifying)
        m_target.write(m_source.read());
}
