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
#include <QSize>
#include <QUrl>

/**
 * @page conf Konfigurační soubor aplikace 
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

    static constexpr const char *DbConstituents          = "db/constituents";

    static constexpr const char *GuiMainWindowSize       = "gui/mainwindow-size";
    static constexpr const char *GuiMainWindowLayout1    = "gui/mainwindow-layout1";
    static constexpr const char *GuiMainWindowLayout2    = "gui/mainwindow-layout2";
    static constexpr const char *GuiConstituentsDialogSize       = "gui/constituents-dialog-size";

    // Logování
    bool    logAll() const { return value(LogAll, true).toBool(); }
    QString logInclude() const { return value(LogInclude).toString(); }
    QString logExclude() const { return value(LogInclude).toString(); }
    QString logFile() const { return value(LogFile).toString(); }

    // DB
    QString dbConstituents() const { return value(DbConstituents, "://data/constituents.json").toString(); }
    void setDbConstituents(const QString& x) { setValue(DbConstituents, x); }

    // GUI
    QSize guiMainWindowSize() { return value(GuiMainWindowSize, QSize(1000, 800)).toSize(); }
    void setGuiMainWindowSize(const QSize& size) { setValue(GuiMainWindowSize, size); }
    QByteArray guiMainWindowLayout1() { return value(GuiMainWindowLayout1).toByteArray(); }
    void setGuiMainWindowLayout1(const QByteArray& layout) { setValue(GuiMainWindowLayout1, layout); }
    QByteArray guiMainWindowLayout2() { return value(GuiMainWindowLayout2).toByteArray(); }
    void setGuiMainWindowLayout2(const QByteArray& layout) { setValue(GuiMainWindowLayout2, layout); }
    QSize guiConstituentsDialogSize() { return value(GuiConstituentsDialogSize, QSize(1000, 996)).toSize(); }
    void setGuiConstituentsDialogSize(const QSize& size) { setValue(GuiConstituentsDialogSize, size); }

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
    MSettings(QObject *parent);
    #ifndef DOXYGEN_SHOULD_SKIP_THIS
    static MSettings *m_instance;
    #endif

};

#endif
