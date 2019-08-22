#pragma once
#ifndef CONSTITUENT_HPP
#define CONSTITUENT_HPP

#include <QList>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QVariantMap>

class Constituent
{
private:
    bool valid;
    int id;
    QString name;
	double U;
	int Z;
	unsigned int negCount;
	unsigned int posCount;
    QVector<double> uNeg;
    QVector<double> uPos;
    QVector<double> pKaNeg;
    QVector<double> pKaPos;
    QVector<double> kaNeg;
    QVector<double> kaPos;
    QVector<double> lNeg; // do samplu?
    QVector<double> lPos; // do samplu?
    double dif;

public:
	Constituent();
    Constituent(const QString& pName, const double pU = 0, const int pZ = 0);

    static const double uFactor;

    ~Constituent();

    void setId(int x) { id = x; }
	void setU(double pU) { U = pU; }
	void setZ(int pZ) { Z = pZ; }
	void addNegU(double pValue);
	void addPosU(double pValue);
	void addNegPKa(double pValue);
	void addPosPKa(double pValue);

    int getId() const { return id; }
    unsigned int getNegCount() const { return negCount; }
    int getNegCharge() const { return -1 * static_cast<int>(negCount); }
    unsigned int getPosCount() const { return posCount; }
    int getPosCharge() const { return static_cast<int>(posCount); }

    double getU() const { return U; }
    double getU(int pCharge) const;
    int getZ() const { return Z; }
    double getL(const int pCharge) const;


    double getPKa(const int pCharge) const;
    double getDif() const { return dif; }
    void show() const;
    const QString& getName() const { return name; }

    QVariantMap json() const;


private:
    void calculateDif();
};


QDebug operator<<(QDebug dbg, const Constituent& other);

Q_DECLARE_METATYPE(Constituent);

#endif
