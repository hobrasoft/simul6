/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */
#ifndef _MSETTINGS_H_
#define _MSETTINGS_H_

#define MSETTINGS MSettings::instance()

#include <QSettings>
#include <QString>
#include <QUrl>

/**
 * @page conf Konfigurační soubor klientské aplikace (Raspberry Pi)
 *
 * Konfigurační soubor se může nalézat na třech různých místech (seřazené dle priority):
 * - ~/.config/hobrasoft.cz/simul.conf
 * - ~/.simul.conf
 * - /etc/simul.conf
 *
 */

/**
 * @brief Globální QSettings objekt
 *
 * Během práce zůstává otevřený a přistupuje se k němu přes makro MSETTINGS.
 * Umožňuje přesunout konfigurační soubor na libovolné místo, například /etc.
 *
 * Singleton instance by měla být přístupná i v QML pod jménem MSETTINGS.
 */
class MSettings : public QSettings {
    Q_OBJECT

  public:
   ~MSettings();
    static MSettings *instance(QObject *parent = nullptr);

    /*
     * Seznam řetězců pro jednotlivé konfigurační parametry
     */
    static constexpr const char *LogAll                  = "log/all";
    static constexpr const char *LogInclude              = "log/include";
    static constexpr const char *LogExclude              = "log/exclude";
    static constexpr const char *LogFile                 = "log/file";

    static constexpr const char *DbFileName              = "db/filename";

    // Logování
    bool    logAll() const { return value(LogAll, true).toBool(); }
    QString logInclude() const { return value(LogInclude).toString(); }
    QString logExclude() const { return value(LogInclude).toString(); }
    QString logFile() const { return value(LogFile).toString(); }

    // DB
    QString dbFilename() const { return value(DbFileName, QString()).toString(); }
    void setDbFilename(const QString& x) { setValue(DbFileName, x); }

  protected:

    /**
     * @brief Nastaví hodnotu v konfiguračním souboru
     *
     * Metoda je přetížená z QSettings::setValue() a přesunutá do protected, aby nebylo možné
     * zavolat tuto metodu přímo z aplikace. Vždy je nutné doplnit pro parametr přístupovou metodu.
     */
    void    setValue(const QString& key, const QVariant& value);

    /**
     * @brief Vrátí hodnotu z konfiguračního souboru
     *
     * Metoda je přetížená z QSettings::value() a přesunutá do protected, aby nebylo možné
     * zavolat tuto metodu přímo z aplikace. Vždy je nutné doplnit pro parametr přístupovou metodu.
     */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

  private:
    /**
     * @brief Konstruktor, otevře zadaný soubor
     */
    MSettings(const QString& filename, QObject *parent);
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    static MSettings *m_instance;
    #endif

};

#endif
