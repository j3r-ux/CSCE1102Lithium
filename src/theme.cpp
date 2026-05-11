#include "theme.h"

#include <QApplication>
#include <QSettings>

namespace {

Theme::Mode currentMode = Theme::Mode::Light;

const char *kLightStyle = R"(
QWidget {
    background-color: #f5f7fa;
    color: #222;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
    font-size: 13px;
}
QMainWindow, QDialog { background-color: #f5f7fa; }

QLabel { color: #2a2a2a; background: transparent; }
QLabel#titleLabel {
    font-size: 26px;
    font-weight: 700;
    color: #1c71d8;
}
QLabel#subtitleLabel { color: #5b6573; }
QLabel#statusLabel { color: #c83232; font-weight: 600; }

QLineEdit {
    background-color: white;
    color: #222;
    border: 1px solid #c8d0db;
    border-radius: 8px;
    padding: 8px 10px;
    selection-background-color: #1c71d8;
    selection-color: white;
}
QLineEdit:focus { border: 2px solid #1c71d8; padding: 7px 9px; }

QTextEdit {
    background-color: white;
    color: #222;
    border: 1px solid #d6dde6;
    border-radius: 8px;
    padding: 6px;
    selection-background-color: #1c71d8;
    selection-color: white;
}

QListWidget {
    background-color: white;
    color: #222;
    border: 1px solid #d6dde6;
    border-radius: 8px;
    padding: 4px;
}
QListWidget::item { padding: 4px 6px; border-radius: 4px; }
QListWidget::item:selected { background-color: #1c71d8; color: white; }

QTabWidget::pane { border: 1px solid #d6dde6; border-radius: 6px; background-color: white; top: -1px; }
QTabBar::tab {
    padding: 6px 14px;
    background-color: #e8edf2;
    color: #444;
    border: 1px solid #d6dde6;
    border-bottom: none;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    margin-right: 2px;
}
QTabBar::tab:selected { background-color: white; color: #1c71d8; font-weight: 600; }
QTabBar::close-button { subcontrol-position: right; }

QPushButton {
    background-color: #1c71d8;
    color: white;
    border: none;
    border-radius: 8px;
    padding: 8px 16px;
    min-height: 28px;
    font-weight: 500;
}
QPushButton:hover    { background-color: #3584e4; }
QPushButton:pressed  { background-color: #155ab4; }
QPushButton:disabled { background-color: #b0bec5; color: #eef; }

QPushButton#SignUp_button, QPushButton#backButton, QPushButton#settingsButton {
    background-color: transparent;
    color: #1c71d8;
    border: 1.5px solid #1c71d8;
}
QPushButton#SignUp_button:hover,
QPushButton#backButton:hover,
QPushButton#settingsButton:hover {
    background-color: rgba(28, 113, 216, 30);
}

QPushButton#disconnectButton, QPushButton#disconnectpushButton {
    background-color: #d44545;
}
QPushButton#disconnectButton:hover, QPushButton#disconnectpushButton:hover {
    background-color: #e05a5a;
}

QGroupBox {
    border: 1px solid #d6dde6;
    border-radius: 10px;
    margin-top: 14px;
    padding-top: 10px;
    font-weight: 600;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    padding: 0 6px;
    color: #1c71d8;
}

QCheckBox { spacing: 6px; color: #2a2a2a; }
QCheckBox::indicator {
    width: 16px; height: 16px;
    border-radius: 4px;
    border: 1px solid #c8d0db;
    background: white;
}
QCheckBox::indicator:checked {
    background-color: #1c71d8;
    border: 1px solid #1c71d8;
}

QSpinBox { background: white; border: 1px solid #c8d0db; border-radius: 6px; padding: 4px 6px; }

QMenuBar { background-color: #f5f7fa; }
QStatusBar { background-color: #f5f7fa; color: #5b6573; }
)";

const char *kDarkStyle = R"(
QWidget {
    background-color: #1e2024;
    color: #e6e6e6;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
    font-size: 13px;
}
QMainWindow, QDialog { background-color: #1e2024; }

QLabel { color: #e6e6e6; background: transparent; }
QLabel#titleLabel {
    font-size: 26px;
    font-weight: 700;
    color: #5fa8ff;
}
QLabel#subtitleLabel { color: #97a2b3; }
QLabel#statusLabel { color: #ff7676; font-weight: 600; }

QLineEdit {
    background-color: #2a2d33;
    color: #e6e6e6;
    border: 1px solid #3a3f48;
    border-radius: 8px;
    padding: 8px 10px;
    selection-background-color: #3584e4;
    selection-color: white;
}
QLineEdit:focus { border: 2px solid #5fa8ff; padding: 7px 9px; }

QTextEdit {
    background-color: #25282d;
    color: #e6e6e6;
    border: 1px solid #3a3f48;
    border-radius: 8px;
    padding: 6px;
    selection-background-color: #3584e4;
    selection-color: white;
}

QListWidget {
    background-color: #25282d;
    color: #e6e6e6;
    border: 1px solid #3a3f48;
    border-radius: 8px;
    padding: 4px;
}
QListWidget::item { padding: 4px 6px; border-radius: 4px; }
QListWidget::item:selected { background-color: #3584e4; color: white; }

QTabWidget::pane { border: 1px solid #3a3f48; border-radius: 6px; background-color: #25282d; top: -1px; }
QTabBar::tab {
    padding: 6px 14px;
    background-color: #2a2d33;
    color: #b0b6bf;
    border: 1px solid #3a3f48;
    border-bottom: none;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    margin-right: 2px;
}
QTabBar::tab:selected { background-color: #25282d; color: #5fa8ff; font-weight: 600; }

QPushButton {
    background-color: #3584e4;
    color: white;
    border: none;
    border-radius: 8px;
    padding: 8px 16px;
    min-height: 28px;
    font-weight: 500;
}
QPushButton:hover    { background-color: #5fa8ff; }
QPushButton:pressed  { background-color: #1c71d8; }
QPushButton:disabled { background-color: #3a3f48; color: #888; }

QPushButton#SignUp_button, QPushButton#backButton, QPushButton#settingsButton {
    background-color: transparent;
    color: #5fa8ff;
    border: 1.5px solid #5fa8ff;
}
QPushButton#SignUp_button:hover,
QPushButton#backButton:hover,
QPushButton#settingsButton:hover {
    background-color: rgba(95, 168, 255, 30);
}

QPushButton#disconnectButton, QPushButton#disconnectpushButton {
    background-color: #c4505e;
}
QPushButton#disconnectButton:hover, QPushButton#disconnectpushButton:hover {
    background-color: #d76773;
}

QGroupBox {
    border: 1px solid #3a3f48;
    border-radius: 10px;
    margin-top: 14px;
    padding-top: 10px;
    font-weight: 600;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    padding: 0 6px;
    color: #5fa8ff;
}

QCheckBox { spacing: 6px; color: #e6e6e6; }
QCheckBox::indicator {
    width: 16px; height: 16px;
    border-radius: 4px;
    border: 1px solid #3a3f48;
    background: #2a2d33;
}
QCheckBox::indicator:checked {
    background-color: #3584e4;
    border: 1px solid #3584e4;
}

QSpinBox { background: #2a2d33; color: #e6e6e6; border: 1px solid #3a3f48; border-radius: 6px; padding: 4px 6px; }

QMenuBar { background-color: #1e2024; color: #e6e6e6; }
QStatusBar { background-color: #1e2024; color: #97a2b3; }
)";

}

namespace Theme {

QString stylesheet(Mode mode)
{
    return mode == Mode::Dark ? QString::fromLatin1(kDarkStyle)
                              : QString::fromLatin1(kLightStyle);
}

void apply(Mode mode)
{
    currentMode = mode;
    if (auto *app = qobject_cast<QApplication*>(QCoreApplication::instance()))
        app->setStyleSheet(stylesheet(mode));
    saveToSettings();
}

Mode current() { return currentMode; }

void loadFromSettings()
{
    QSettings s("Lithium", "Lithium");
    const bool dark = s.value("theme/dark", false).toBool();
    apply(dark ? Mode::Dark : Mode::Light);
}

void saveToSettings()
{
    QSettings s("Lithium", "Lithium");
    s.setValue("theme/dark", currentMode == Mode::Dark);
}

}
