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
    QColor  m_color;
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
	void setColor(const QColor& c) { m_color = c; }
	void addNegU(double pValue);
	void addPosU(double pValue);
	void addNegPKa(double pValue);
	void addPosPKa(double pValue);

    int getId() const { return m_id; }
    unsigned int getNegCount() const { return m_negCount; }
    int getNegCharge() const { return -1 * static_cast<int>(m_negCount); }
    unsigned int getPosCount() const { return m_posCount; }
    int getPosCharge() const { return static_cast<int>(m_posCount); }

    double getU() const { return m_U; }
    double getU(int pCharge) const;
    int getZ() const { return m_Z; }
    double getL(const int pCharge) const;
    QColor color() const { return m_color; }


    double getPKa(const int pCharge) const;
    double getDif() const { return m_dif; }
    void show() const;
    const QString& getName() const { return m_name; }

    QVariantMap json() const;


private:
    void calculateDif();
};


QDebug operator<<(QDebug dbg, const Constituent& other);

Q_DECLARE_METATYPE(Constituent);

#endif
