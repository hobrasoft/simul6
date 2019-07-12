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
 * - ~/.config/hobrasoft.cz/robept.conf
 * - ~/.robept.conf
 * - /etc/robept.conf
 *
 * Pokud běží aplikace jako služba spuštěná přes systemd, používá se vždy soubor /etc/robept.conf.
 *
 * @section conf-p Konfigurační parametry
 *
 *
 * @subsection conf-p-api [api]
 * Parametry pro přístup k api. V ostrém provozu by mělo stačit nastavit pouze _host_.
 *
 * - scheme - http nebo https (default https)
 * - port - port (default 443)
 * - path - cesta k API (default /)
 * - version - verze API (default v1)
 * - key - cesta ke klíci pro autentizaci (default /etc/ssl/robept.crt)
 * - certificate - cesta k certifikátu pro autentizaci (default /etc/ssl/robept.key)
 * - cacertificate - cesta k certifikátu certifikační autority pro autentizaci (default /etc/ssl/robeptca.crt)
 *
 * @subsection conf-p-log [log]
 * Parametry pro nastavení logování. Aplikace dovede logovat do souboru a lze velmi detailně nastavit,
 * co se má logovat.
 *
 * - all - pokud je true, loguje se vše
 * - file - cesta k souboru, pokud není zadané, loguje na stdout
 * - include - co se má do logu zahrnout, regulární výraz. Má smysl, pokud je all nastaveno na false.
 * - exclude - co se nemá do logu zahrnout, regulární výraz
 *
 * @subsection conf-p-gui [gui]
 * Parametry pro nastavení gui.
 *
 * - autoLogin - pokud je zadaný platný QRCODE uživatele, je tento uživatel přihlášený automaticky
 *
 * @subsection conf-p-operations [operations]
 * Parametry pro nastavení operací řízených QR kódem. Některé formulářové položky je možné
 * zvolit jak kliknutím na obrazovku, tak načtením speciálního QR kódu. Jedná se o operace
 * změny režimu vkládání (nové, spojit a rozpojit) a o odhlášení uživatele. Operace mají 
 * své výchozí názvy, v konfiguračním souboru je lze změnit:
 *
 *
 * - new - Přepne na režim "Nové" (default -operation-new)
 * - assemble - Přepne na režim "Připojit" (default -operation-assemble)
 * - disassemble - Přepne na režim "Odpojit" (default -operation-disassemble)
 * - logout - Odhlásí uživatele (default -operation-logout)
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
    Q_PROPERTY(QString  fileName                READ fileName);
    Q_PROPERTY(bool     logAll                  READ logAll);
    Q_PROPERTY(QString  logInclude              READ logInclude);
    Q_PROPERTY(QString  logExclude              READ logExclude);
    Q_PROPERTY(QString  logFile                 READ logFile);
    Q_PROPERTY(QUrl     apiUrl                  READ apiUrl);
    Q_PROPERTY(QString  apiCertificate          READ apiCertificate);
    Q_PROPERTY(QString  apiCaCertificate        READ apiCaCertificate);
    Q_PROPERTY(QString  apiKey                  READ apiKey);
    Q_PROPERTY(QString  autoLogin               READ autoLogin);
    Q_PROPERTY(QString  neshodaURL              READ neshodaURL);
    Q_PROPERTY(QString  operationLogout         READ operationLogout);
    Q_PROPERTY(QString  operationNew            READ operationNew);
    Q_PROPERTY(QString  operationAssemble       READ operationAssemble);
    Q_PROPERTY(QString  operationDisassemble    READ operationDisassemble);
    Q_PROPERTY(bool     clockAnalog             READ clockAnalog);
    Q_PROPERTY(bool     showComponentListHeader READ showComponentListHeader);
    Q_PROPERTY(bool     allowEmptyCategory      READ allowEmptyCategory);

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

    static constexpr const char *ApiScheme               = "api/scheme";
    static constexpr const char *ApiHost                 = "api/host";
    static constexpr const char *ApiPort                 = "api/port";
    static constexpr const char *ApiPath                 = "api/path";
    static constexpr const char *ApiVersion              = "api/version";
    static constexpr const char *ApiCertificate          = "api/certificate";
    static constexpr const char *ApiCaCertificate        = "api/cacertificate";
    static constexpr const char *ApiKey                  = "api/key";

    static constexpr const char *RobeAutoLogin           = "robe/autologin";
    static constexpr const char *RobeNeshodaURL          = "robe/neshodaURL";
    static constexpr const char *UpgradeScript           = "plukovnik/upgrade-script";

    static constexpr const char *OperationNew            = "operations/new";
    static constexpr const char *OperationAssemble       = "operations/assemble";
    static constexpr const char *OperationDisassemble    = "operations/disassemble";
    static constexpr const char *OperationLogout         = "operations/logout";

    static constexpr const char *WifiBootConf            = "wifi/boot-conf";
    static constexpr const char *WifiEtcConf             = "wifi/etc-conf";

    static constexpr const char *ClockAnalog             = "gui/clockAnalog";
    static constexpr const char *ShowComponentListHeader = "gui/showComponentListHeader";
    static constexpr const char *AllowEmptyCategory      = "gui/allowEmptyCategory";

    // Logování
    bool    logAll() const { return value(LogAll, true).toBool(); }
    QString logInclude() const { return value(LogInclude).toString(); }
    QString logExclude() const { return value(LogInclude).toString(); }
    QString logFile() const { return value(LogFile).toString(); }

    // API
    QString apiScheme() const { return value(ApiScheme, "https").toString(); }
    QString apiHost() const { return value(ApiHost).toString(); }
    int     apiPort() const { return value(ApiPort, 443).toInt(); }
    QString apiPath() const { return value(ApiPath, "/").toString(); }
    QString apiVersion() const { return value(ApiVersion, "/v1").toString(); }
    QString apiCertificate() const { return value(ApiCertificate, "/etc/ssl/robept.crt" ).toString(); }
    QString apiCaCertificate() const { return value(ApiCaCertificate, "/etc/ssl/robeptca.crt" ).toString(); }
    QString apiKey() const { return value(ApiKey, "/etc/ssl/robept.key").toString(); }
    QUrl apiUrl() const { 
                QString path = "/" + apiPath() + "/" + apiVersion() + "/";
                path = path.replace("///", "/");
                path = path.replace("//", "/");
                QUrl url; 
                url.setScheme(apiScheme()); 
                url.setHost(apiHost()); 
                url.setPort(apiPort()); 
                url.setPath(path);
                return url;
                }

    // Robe
    QString autoLogin() const { return value(RobeAutoLogin, QString()).toString(); }
    QString neshodaURL() const { return value(RobeNeshodaURL, "http://potkan.hobrasoft.cz/doc/robe-pt").toString(); }

    // General
    QString upgradeScript() const { return value(UpgradeScript, "/usr/bin/robe-pt-upgrade.sh").toString(); }

    // Operations
    QString operationNew() const { return value(OperationNew, "-operation-new").toString(); }
    QString operationAssemble() const { return value(OperationAssemble, "-operation-assemble").toString(); }
    QString operationDisassemble() const { return value(OperationDisassemble, "-operation-disassemble").toString(); }
    QString operationLogout() const { return value(OperationLogout, "-operation-logout").toString(); }

    // Wifi
    QString wifiBootConf() const { return value(WifiBootConf, "/boot/wpa_supplicant.conf").toString(); }
    QString wifiEtcConf() const { return value(WifiEtcConf, "/etc/wpa_supplicant/wpa_supplicant.conf").toString(); }

    // GUI
    bool clockAnalog() const { return value(ClockAnalog, false).toBool(); }
    bool showComponentListHeader() const { return value(ShowComponentListHeader, false).toBool(); }
    bool allowEmptyCategory() const { return value(AllowEmptyCategory, false).toBool(); }

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
