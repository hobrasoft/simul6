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
    static constexpr const char *LogAll                     = "log/all";
    static constexpr const char *LogInclude                 = "log/include";
    static constexpr const char *LogExclude                 = "log/exclude";
    static constexpr const char *LogFile                    = "log/file";

    static constexpr const char *DbConstituents             = "db/constituents";

    static constexpr const char *GuiMainWindowSize          = "gui/mainwindow-size";
    static constexpr const char *GuiMainWindowLayout        = "gui/mainwindow-layout";
    static constexpr const char *GuiConstituentsDialogSize  = "gui/constituents-dialog-size";
    static constexpr const char *GuiDataDirName             = "gui/data-dir-name";
    static constexpr const char *GuiExportDirName           = "gui/export-dir-name";
    static constexpr const char *GuiAmpholinesWindowSize    = "gui/ampholines-window-size";
    static constexpr const char *GuiChartAntialiasing       = "gui/chart-antialiasing";
    static constexpr const char *GuiFloatable               = "gui/floatable";
    static constexpr const char *GuiCloseable               = "gui/closeable";
    static constexpr const char *GuiMoveable                = "gui/moveable";
    static constexpr const char *GuiDockCompositionGeometry = "gui/dock-composition-geometry";

    static constexpr const char *DbConstituentsDefault   = "://data/constituents.json";

    // Logování
    bool    logAll() const { return value(LogAll, true).toBool(); }
    QString logInclude() const { return value(LogInclude).toString(); }
    QString logExclude() const { return value(LogInclude).toString(); }
    QString logFile() const { return value(LogFile).toString(); }

    // DB
    QString dbConstituents() const { return value(DbConstituents, DbConstituentsDefault).toString(); }
    void setDbConstituents(const QString& x) { setValue(DbConstituents, x); }

    // GUI
    QSize guiMainWindowSize() const { return value(GuiMainWindowSize, QSize(1000, 800)).toSize(); }
    void setGuiMainWindowSize(const QSize& size) { setValue(GuiMainWindowSize, size); }
    QByteArray guiMainWindowLayout() const { return value(GuiMainWindowLayout).toByteArray(); }
    void setGuiMainWindowLayout(const QByteArray& layout) { setValue(GuiMainWindowLayout, layout); }
    QByteArray guiDockCompositionGeometry() const { return value(GuiDockCompositionGeometry).toByteArray(); }
    void setGuiDockCompositionGeometry(const QByteArray& geometry) { setValue(GuiDockCompositionGeometry, geometry); }
    QSize guiConstituentsDialogSize() const { return value(GuiConstituentsDialogSize, QSize(1000, 996)).toSize(); }
    void setGuiConstituentsDialogSize(const QSize& size) { setValue(GuiConstituentsDialogSize, size); }
    QString dataDirName() const { return value(GuiDataDirName).toString(); }
    void setDataDirName(const QString& dirname) { setValue(GuiDataDirName, dirname); }
    QString exportDirName() const { return value(GuiExportDirName).toString(); }
    void setExportDirName(const QString& dirname) { setValue(GuiExportDirName, dirname); }
    QSize guiAmpholinesWindowSize() const { return value(GuiAmpholinesWindowSize, QSize(800, 600)).toSize(); }
    void setGuiAmpholinesWindowSize(const QSize& size) { setValue(GuiAmpholinesWindowSize, size); }
    bool guiChartAntialiasing() const { return value(GuiChartAntialiasing, true).toBool(); }
    void setGuiChartAntialiasing(bool x) { setValue(GuiChartAntialiasing, x); }
    bool guiFloatable() const { return value(GuiFloatable, false).toBool(); }
    void setGuiFloatable(bool x) { setValue(GuiFloatable, x); }
    bool guiMoveable() const { return value(GuiMoveable, false).toBool(); }
    void setGuiMoveable(bool x) { setValue(GuiMoveable, x); }
    bool guiCloseable() const { return value(GuiCloseable, false).toBool(); }
    void setGuiCloseable(bool x) { setValue(GuiCloseable, x); }

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
