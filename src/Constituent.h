#pragma once
#ifndef CONSTITUENT_HPP
#define CONSTITUENT_HPP

#include <QList>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QVariantMap>
#include <QColor>

class Constituent
{
private:
    bool m_valid;
    int m_id;
    QString m_name;
	double m_U;
	int m_Z;
	unsigned int m_negCount;
	unsigned int m_posCount;
    QVector<double> m_uNeg;
    QVector<double> m_uPos;
    QVector<double> m_pKaNeg;
    QVector<double> m_pKaPos;
    QVector<double> m_kaNeg;
    QVector<double> m_kaPos;
    QVector<double> m_lNeg; // do samplu?
    QVector<double> m_lPos; // do samplu?
    double m_dif;

public:
	Constituent();
    Constituent(const QString& pName, const double pU = 0, const int pZ = 0);
    Constituent(const QVariantMap& json);

    static const double uFactor;

    ~Constituent();

    void setId(int x) { m_id = x; }
	void setU(double pU) { m_U = pU; }
	void setZ(int pZ) { m_Z = pZ; }
	void addNegU(double pValue);
	void addPosU(double pValue);
	void addNegPKa(double pValue);
	void addPosPKa(double pValue);

    int inline getId() const { return m_id; }
    unsigned int inline getNegCount() const { return m_negCount; }
    int inline getNegCharge() const { return -1 * static_cast<int>(m_negCount); }
    unsigned int inline getPosCount() const { return m_posCount; }
    int inline getPosCharge() const { return static_cast<int>(m_posCount); }

    double inline getU() const { return m_U; }
    int inline getZ() const { return m_Z; }
    double inline getL(const int pCharge) const {
        if (pCharge < 0) {
            return m_lNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
            }
        if (pCharge > 0) {
            return m_lPos[static_cast<unsigned int>(pCharge) - 1];
            }
        return 1;
        }
    double inline getU(int pCharge) const {
        unsigned int sign;
        if (pCharge < 0) {
            sign =static_cast<unsigned int>(-1 * pCharge - 1);
            if (sign < m_negCount) {
                return m_uNeg[sign];
                }
          } else {
            sign = static_cast<unsigned int>(pCharge - 1);
            if (sign < m_posCount) {
                return m_uPos[sign];
                }
            }
        return 0;
        }


    double inline getPKa(const int pCharge) const {
        if (pCharge < 0) {
            return m_pKaNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
          } else if (pCharge > 0) {
            return m_pKaPos[static_cast<unsigned int>(pCharge) - 1];
          } else {
            return 1;
            }
        }

    double inline getDif() const { return m_dif; }
    void show() const;
    inline const QString& getName() const { return m_name; }

    QVariantMap json() const;
    QVariantMap jsonSimplified() const;


private:
    void calculateDif();
    friend QDebug operator<<(QDebug dbg, const Constituent& other);
};


QDebug operator<<(QDebug dbg, const Constituent& other);

Q_DECLARE_METATYPE(Constituent);

#endif
