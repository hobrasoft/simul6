#include "importsna.h"
#include "pdebug.h"
#include <QFile>


ImportSna::ImportSna(QObject *parent) : QObject(parent) {
    m_file = nullptr;
}


void ImportSna::openFile() {
    m_file = new QFile(m_filename, this);
    if (!m_file->open(QIODevice::ReadOnly)) {
        throw(tr("Could not open %1").arg(m_filename));
        }
}

QString ImportSna::readString() {
    QByteArray buf;
    char c;
    do {
        if (!m_file->getChar(&c)) {
            throw(tr("Could not read bytes from %1").arg(m_filename));
            }
        buf.append(c);
        } while (c != 0);
    return QString::fromUtf8(buf);
}


double ImportSna::readDouble() {
    double x;
    if (-1 == m_file->read( (char *)(&x), sizeof(x) )) {
        throw(tr("Could not read double from %1").arg(m_filename));
        }
    return x;
}


bool ImportSna::readBool() {
    bool x;
    if (-1 == m_file->read( (char *)(&x), sizeof(x) )) {
        throw(tr("Could not read bool from %1").arg(m_filename));
        }
    return x;
}


int ImportSna::readInt() {
    int x;
    if (-1 == m_file->read( (char *)(&x), sizeof(x) )) {
        throw(tr("Could not read integer from %1").arg(m_filename));
        }
    return x;
}


QVariant ImportSna::readData(const QString& filename) {
    QVariantMap data;
    m_filename = filename;

    try {
        openFile();
        data["sign"]    = readString();
        data["version"] = readString();
        data["time"] = readDouble();
        data["dt"] = readDouble();
        data["errOpt"] = readDouble();
        data["voltage"] = readDouble();
        data["EIOF_u"] = readDouble();
        data["left_Wall"] = readInt();
        data["right_Wall"] = readInt();
        readBool();
        readBool();
        readBool();
        readBool();
        readBool();
        readBool();
        readBool();
        readInt(); // NX_MAX 
        readDouble(); // width MM
        data["zone_edge"] = readDouble() / 1000.0;
        readDouble(); // inj_site_mm
        data["caplen"] = readDouble();
        readBool();
        readBool();
        readBool();
        readDouble(); // lw lim
        readDouble(); // rw lim
        readInt(); // wall inc
        readBool(); // power mode
        readDouble(); // cur_density || voltage
        readBool(); // iter PSQ
        readInt(); // PSQ mode
        readBool(); // cbLBorder
        readBool(); // cbRBorder
        readBool(); // cbGauss
        readBool(); // cbDCDTcor
        readBool(); // cbPCIS
        readInt(); // iterations newton's method
        readDouble(); // eps new
        readDouble(); // eps Cor
        readBool(); // cbSDS
        readBool(); // cb Show Mix
        readBool(); // cb Show CMC
        readDouble(); // dcn
        readDouble(); // u_mic
        readDouble(); // CMC_cntr
        readDouble(); // CMC_L
        readDouble(); // CMC_R
        readDouble(); // CMC_W
        readDouble(); // omega
        readBool(); // diff precond on
        readDouble(); // precond time cnt
        readDouble(); // precond duration
        readDouble(); // set precond duration
        readDouble(); // dt pre diff
        readDouble(); // inc pre diff
        readBool(); // no use
        readBool(); // no use
        readBool(); // no use
        readBool(); // no use
        readBool(); // no use
        readInt(); // no use
        readInt(); // no use
        readInt(); // no use
        readInt(); // no use
        readInt(); // no use
        readDouble(); // no use
        readDouble(); // no use
        readDouble(); // no use
        readDouble(); // no use
        readDouble(); // no use
        if (data["version"].toString() == "1.0") {
            // ???
            }
        if (data["version"].toString() == "1.1") {
            readBool();
            readBool();
            readBool();
            readBool();
            int nr = readInt(); // NR Rows
            int ns = readInt(); // NS
            // PDEBUG << nr;
            // PDEBUG << ns;
            // PDEBUG << readBool();
            // PDEBUG << readBool();
            // PDEBUG << readBool();
            // PDEBUG << readBool();
            // PDEBUG << readBool();
            // PDEBUG << readBool();
            if (ns > 0 || nr > 0) {
                throw(tr("Unsupported SNA file format"));
                }
            // PDEBUG << "dateTime" << readDouble();
            // PDEBUG << "realTime" << readDouble();
            // PDEBUG << "count main" << readInt();
            // PDEBUG << "count elneut" << readInt();
            }
        }
    catch (const QString& errstring) {
        // PDEBUG << errstring;
        return QVariant();
        }

    // PDEBUG << data;

    return data;
}


