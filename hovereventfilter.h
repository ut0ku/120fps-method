#ifndef HOVEREVENTFILTER_H
#define HOVEREVENTFILTER_H

#include <QObject>
#include <QEvent>
#include <functional>

class HoverEventFilter : public QObject
{
    Q_OBJECT
public:
    HoverEventFilter(QObject* parent, std::function<void(bool)> callback)
        : QObject(parent), m_callback(callback) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::Enter) {
            m_callback(true);
        } else if (event->type() == QEvent::Leave) {
            m_callback(false);
        }
        return QObject::eventFilter(obj, event);
    }

private:
    std::function<void(bool)> m_callback;
};

#endif // HOVEREVENTFILTER_H
