/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ColorsGenerator_H_
#define _ColorsGenerator_H_

#include <QColor>

class ColorsGenerator {
public:
    static QColor color(int index) {
        double h = 255 * 0.618033988 * index;
        int hi = ((int)h)%255;
        return QColor::fromHsv(hi, 245, 245, 255);
        }

    static QColor color() {
        QColor c = color(lastIndex);
        lastIndex += 1;
        return c;
        }

    static void reset() {
        lastIndex = 0;
        }

private:
    static int lastIndex;
};

#endif

