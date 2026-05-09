#ifndef THEME_H
#define THEME_H

#include <QString>

namespace Theme {

enum class Mode { Light, Dark };

QString stylesheet(Mode mode);
void    apply(Mode mode);
Mode    current();

void    loadFromSettings();
void    saveToSettings();

}

#endif
