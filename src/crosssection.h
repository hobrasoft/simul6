/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Crosssection_H_
#define _Crosssection_H_

/**
 * @brief
 */
struct Crosssection {
    struct Section {
        double ratio;
        double diameter;
        Section(double r, double d) { ratio = r; diameter = d; }
        };
    QList<Section> sections;
    Crosssection& operator<<(const Section& x) { sections << x; return *this; }
    int ratioSum() const {
        int sum = 0;
        for (int i=0; i<sections.size(); i++) {
            sum += sections[i].ratio;
            }
        return sum;
        }
    int size() const { return sections.size(); }
    const Section& operator[](int i) const { return sections[i]; }
};

#endif

