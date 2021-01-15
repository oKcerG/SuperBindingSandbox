import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.animation 1.0
import SuperBinding 1.0
import SlowBackend 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    SlowBackend {
        id: slowBackend
        directValue: 0.5
        BoundaryRule on value {
            minimum: 0.2
            maximum: 0.8
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 20
        Slider {
            id: backend
            value: 0.5
            BoundaryRule on value {
                minimum: 0.2
                maximum: 0.8
            }
        }
        Slider {
            id: frontend
            SuperBinding on value{
                source: qtProperty(backend, "value")
                modifying: frontend.pressed
            }
        }

        Slider {
            id: slowBackendSlider
            value: slowBackend.directValue
            onMoved: slowBackend.directValue = value
        }
        Slider {
            SuperBinding on value{
                source: qtProperty(slowBackend, "value")
                modifying: target.object.pressed
                delayMode: SuperBinding.DelayThrottle
            }
        }
        Slider {
            SuperBinding on value{
                source: qtProperty(slowBackend, "value")
                modifying: target.object.pressed
                delayMode: SuperBinding.DelayThrottle
                timeout: 500
                delay: 200
            }
        }
    }
}
