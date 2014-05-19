/***************************************************************************
  basicsettingspage.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef BASICSETTINGSPAGE_H
#define BASICSETTINGSPAGE_H

#include <QWidget>
#include <QDateTime>

class QLabel;
class QPushButton;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QDateEdit;
class QTimeEdit;
class QDate;
class QTime;
class QCalendarWidget;
class QNetworkAccessManager;
class QNetworkReply;
class QRadioButton;
class QButtonGroup;

class QwwButtonLineEdit;
class QwwClearLineEdit;
class QProgressIndicator;

class ClickLabel;
class DlProject;
class EcProject;
class AnemDesc;
class IrgaDesc;
class VariableDesc;
class RawFilenameDialog;
struct BiomItem;
class SmartFluxBar;
struct ConfigState;

class BasicSettingsPage : public QWidget
{
    Q_OBJECT

public:
    BasicSettingsPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config);
    virtual ~BasicSettingsPage();

    QwwButtonLineEdit *datapathEdit;
    QwwClearLineEdit *outpathEdit;
    QwwClearLineEdit *idEdit;
    QwwButtonLineEdit *previousDatapathEdit;

    enum EmbeddedFileFlag
    {
        rawEmbeddedFile    = 1,
        biometEmbeddedFile = 2
    };
    Q_DECLARE_FLAGS(EmbeddedFileFlags, EmbeddedFileFlag)

    void updateSmartfluxBar();
public slots:
    void refresh();
    void datapathBrowse_clicked();
    void outpathBrowse_clicked();
    void previousDatapathBrowse_clicked();
    void askRawFilenameFormat();
    void updateMetadataRead(bool firstReading = false);
    void updateRawFilenameFormat();
    void filesFoundClear();
    void updateProjectFilesFound(int fileNumber);
    void clearSelectedItems();

protected:
    bool eventFilter(QObject* watched, QEvent* event);

private:
    static const QString FLAG_POLICY_STRING_0;
    static const QString FLAG_POLICY_STRING_1;

    QList<bool> oldEnabled;

    QPushButton* questionMark_1;
    QPushButton* questionMark_2;
    QPushButton* questionMark_3;
    QPushButton* questionMark_4;
    QPushButton* questionMark_5;

    ClickLabel *datapathLabel;
    QPushButton *datapathBrowse;
    QCheckBox *recursionCheckBox;
    QLabel* filesFound;
    QProgressIndicator* progressWidget_1;
    QProgressIndicator* progressWidget_2;

    ClickLabel *idLabel;

    ClickLabel *outpathLabel;
    QPushButton *outpathBrowse;

    ClickLabel *previousDatapathLabel;
    QPushButton *previousDatapathBrowse;

    ClickLabel *nFileLabel;
    QSpinBox *nFilesSpin;

    ClickLabel *lockedIcon_2;

    ClickLabel *avgIntervalLabel;
    QSpinBox *avgIntervalSpin;

    ClickLabel *maxLackLabel;
    QSpinBox *maxLackSpin;

    QRadioButton* useMagneticNRadio;
    QRadioButton* useGeographicNRadio;
    QButtonGroup* northRadioGroup;

    QCheckBox *subsetCheckBox;
    QPushButton* dateRangeDetectButton;
    QWidget* moreSubsetContainer;
    ClickLabel *startDateLabel;
    ClickLabel *endDateLabel;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QTimeEdit *startTimeEdit;
    QTimeEdit *endTimeEdit;
    QLabel* lockedIcon_1;

    QCheckBox *crossWindCheckBox;

    ClickLabel *fileFormatLabel;
    QwwButtonLineEdit *fileFormatEdit;
    QPushButton *fileFormatSetButton;

    ClickLabel* anemRefLabel;
    QComboBox* anemRefCombo;
    ClickLabel* tsRefLabel;
    QComboBox* tsRefCombo;
    ClickLabel* co2RefLabel;
    QComboBox* co2RefCombo;
    ClickLabel* h2oRefLabel;
    QComboBox* h2oRefCombo;
    ClickLabel* ch4RefLabel;
    QComboBox* ch4RefCombo;
    ClickLabel* fourthGasRefLabel;
    QComboBox* fourthGasRefCombo;
    QWidget *gasExtension;
    ClickLabel* gasMwLabel;
    QDoubleSpinBox *gasMw;
    ClickLabel* gasDiffLabel;
    QDoubleSpinBox *gasDiff;
    QPushButton* moreButton;
    ClickLabel* intTcRefLabel;
    QComboBox* intTcRefCombo;
    ClickLabel* intT1RefLabel;
    QComboBox* intT1RefCombo;
    ClickLabel* intT2RefLabel;
    QComboBox* intT2RefCombo;
    ClickLabel* intPRefLabel;
    QComboBox* intPRefCombo;
    ClickLabel* airTRefLabel;
    QComboBox* airTRefCombo;
    ClickLabel* airPRefLabel;
    QComboBox* airPRefCombo;
    ClickLabel* rhLabel;
    QComboBox* rhCombo;
    ClickLabel* rgLabel;
    QComboBox* rgCombo;
    ClickLabel* lwinLabel;
    QComboBox* lwinCombo;
    ClickLabel* ppfdLabel;
    QComboBox* ppfdCombo;
    ClickLabel* diag7500Label;
    QComboBox* diag7500Combo;
    ClickLabel* diag7200Label;
    QComboBox* diag7200Combo;
    ClickLabel* diag7700Label;
    QComboBox* diag7700Combo;
    ClickLabel* flag1Label;
    QComboBox* flag1VarCombo;
    QLabel* flag1UnitLabel;
    QDoubleSpinBox* flag1ThresholdSpin;
    QComboBox* flag1PolicyCombo;
    ClickLabel* flag2Label;
    QComboBox* flag2VarCombo;
    QLabel* flag2UnitLabel;
    QDoubleSpinBox* flag2ThresholdSpin;
    QComboBox* flag2PolicyCombo;
    ClickLabel* flag3Label;
    QComboBox* flag3VarCombo;
    QLabel* flag3UnitLabel;
    QDoubleSpinBox* flag3ThresholdSpin;
    QComboBox* flag3PolicyCombo;
    ClickLabel* flag4Label;
    QComboBox* flag4VarCombo;
    QLabel* flag4UnitLabel;
    QDoubleSpinBox* flag4ThresholdSpin;
    QComboBox* flag4PolicyCombo;
    ClickLabel* flag5Label;
    QComboBox* flag5VarCombo;
    QLabel* flag5UnitLabel;
    QDoubleSpinBox* flag5ThresholdSpin;
    QComboBox* flag5PolicyCombo;
    ClickLabel* flag6Label;
    QComboBox* flag6VarCombo;
    QLabel* flag6UnitLabel;
    QDoubleSpinBox* flag6ThresholdSpin;
    QComboBox* flag6PolicyCombo;
    ClickLabel* flag7Label;
    QComboBox* flag7VarCombo;
    QLabel* flag7UnitLabel;
    QDoubleSpinBox* flag7ThresholdSpin;
    QComboBox* flag7PolicyCombo;
    ClickLabel* flag8Label;
    QComboBox* flag8VarCombo;
    QLabel* flag8UnitLabel;
    QDoubleSpinBox* flag8ThresholdSpin;
    QComboBox* flag8PolicyCombo;
    ClickLabel* flag9Label;
    QComboBox* flag9VarCombo;
    QLabel* flag9UnitLabel;
    QDoubleSpinBox* flag9ThresholdSpin;
    QComboBox* flag9PolicyCombo;
    ClickLabel* flag10Label;
    QComboBox* flag10VarCombo;
    QLabel* flag10UnitLabel;
    QDoubleSpinBox* flag10ThresholdSpin;
    QComboBox* flag10PolicyCombo;

    DlProject *dlProject_;
    EcProject *ecProject_;
    ConfigState* configState_;

    RawFilenameDialog* rawFilenameDialog;

    QLabel* northLabel;
    ClickLabel* declinationLabel;
    QLineEdit* declinationEdit;
    ClickLabel* declinationDateLabel;
    QDateEdit* declinationDateEdit;
    QLabel* decChangingLabel;
    QPushButton* declinationFetchButton;

    QNetworkAccessManager *httpManager_;
    QNetworkReply *httpReply_;
    QByteArray httpBuffer_;
    QString declination_;
    QProgressIndicator* progressWidget_3;

    QStringList currentRawDataList_;

    QList<BiomItem> biomList_;

    QString lastEmbeddedMdFileRead_;

    SmartFluxBar* smartfluxBar_;

    void captureEmbeddedMetadata(BasicSettingsPage::EmbeddedFileFlags type);
    void addNoneStr_1();
    void addNoneStr_2();
    void clearVarsCombo();
    void clearBiometCombo();
    void clearFlagVars();
    void clearFlagUnits();
    void clearFlagThresholdsAndPolicies();
    void filterVariables();
    void preselectVariables();
    void preselectDensityVariables(QComboBox* combo);
    void preselect7700Variables(QComboBox* combo);

    void createQuestionMark();

    void readEmbeddedMetadata(const QString& mdFile);
    void readAlternativeMetadata(const QString &mdFile, bool firstReading = false);

    void readBiomEmbMetadata(const QString& mdFile);
    bool readBiomAltMetadata(const QString& mdFile);

    void reloadSelectedItems_1();
    void reloadSelectedItems_2();

    int getSuggestedFilesToMerge();

    void forceEndDatePolicy();
    void forceEndTimePolicy();

    void updateFilesFoundLabel(int fileNumber);

    QString getFlagUnit(const VariableDesc& varStr);

    void parseMetadataProject(bool isEmbedded);
    void parseBiomMetadata();

    void setSmartfluxUI(bool on);

private slots:
    void updateDataPath(const QString& dp);
    void updateRecursion(bool b);
    void updateOutPath(const QString& dp);
    void updatePreviousDataPath(const QString& dp);
    void updateNFiles(int n);
    void updateAvrgLen(int n);
    void updateMaxLack(int n);
    void onFileFormatLabelClicked();
    void updateFilePrototype_1(const QString& f);
    void updateFilePrototype_2(const QString& f);
    void updateSubsetSelection(bool b);

    void onDatapathLabelClicked();
    void onOutpathLabelClicked();
    void onPreviousDatapathLabelClicked();
    void onIdLabelClicked();
    void updateId(const QString& id);
    void onAvgLenLabelClicked();
    void onMaxLackLabelClicked();
    void onStartDateLabelClicked();
    void onEndDateLabelClicked();
    void updateStartDate(const QDate &d);
    void updateEndDate(const QDate &d);
    void updateStartTime(const QTime &t);
    void updateEndTime(const QTime &t);

    void onClickAnemRefLabel();
    void onClickCo2RefLabel();
    void onClickH2oRefLabel();
    void onClickCh4RefLabel();
    void onClickFourthGasRefLabel();
    void onClickIntTcRefLabel();
    void onClickIntT1RefLabel();
    void onClickIntT2RefLabel();
    void onClickIntPRefLabel();
    void onClickAirTRefLabel();
    void onClickAirPRefLabel();
    void onClickRhLabel();
    void onClickRgLabel();
    void onClickLwinLabel();
    void onClickPpfdLabel();
    void onClickDiag7500Label();
    void onClickDiag7200Label();
    void onClickDiag7700Label();
    void onClickTsRefLabel();
    void updateAnemRefCombo(const QString& s);
    void updateCo2RefCombo(int);
    void updateH2oRefCombo(int);
    void updateCh4RefCombo(int);
    void updateGasMw(double value);
    void updateGasDiff(double value);
    void updateFourthGasRefCombo(int);
    void updateFourthGasSettings(const QString& s);
    void updateIntTcRefCombo(int i);
    void updateIntT1RefCombo(int i);
    void updateIntT2RefCombo(int);
    void updateIntPRefCombo(int i);
    void updateAirTRefCombo(int i);
    void updateAirPRefCombo(int);
    void updateRhCombo(int);
    void updateRgCombo(int);
    void updateLwinCombo(int);
    void updatePpfdCombo(int);
    void updateDiag7500Combo(int i);
    void updateDiag7200Combo(int);
    void updateDiag7700Combo(int i);
    void updateTsRefCombo(int i);

    void updateFlag1Combo(int i);
    void updateFlag2Combo(int i);
    void updateFlag3Combo(int i);
    void updateFlag4Combo(int i);
    void updateFlag5Combo(int i);
    void updateFlag6Combo(int i);
    void updateFlag7Combo(int i);
    void updateFlag8Combo(int i);
    void updateFlag9Combo(int i);
    void updateFlag10Combo(int i);

    void updateFlagUnit(int i);

    void updateCrossWind(bool b);

    void onClickFlagLabel();

    void updateFlag1Threshold(double n);
    void updateFlag2Threshold(double n);
    void updateFlag3Threshold(double n);
    void updateFlag4Threshold(double n);
    void updateFlag5Threshold(double n);
    void updateFlag6Threshold(double n);
    void updateFlag7Threshold(double n);
    void updateFlag8Threshold(double n);
    void updateFlag9Threshold(double n);
    void updateFlag10Threshold(double n);

    void updateFlag1Policy(int n);
    void updateFlag2Policy(int n);
    void updateFlag3Policy(int n);
    void updateFlag4Policy(int n);
    void updateFlag5Policy(int n);
    void updateFlag6Policy(int n);
    void updateFlag7Policy(int n);
    void updateFlag8Policy(int n);
    void updateFlag9Policy(int n);
    void updateFlag10Policy(int n);

    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_5();

    void triggerGasProperties();

    void reset();

    void updateCrossWindCheckBox(const QString& model);
    void updateFilesFound(bool recursionToggled);
    void runUpdateFilesFound();

    void fetchMagneticDeclination();
    void replyFinished(QNetworkReply* reply);
    void bufferHttpReply();
    bool parseHttpReply(const QByteArray& data);

    void northRadioClicked(int b);
    void updateMagDec(const QString& dec);
    double numDeclination(const QString &text);
    QString strDeclination(double dec);
    QString strVariation(double dec);
    void onClickDeclinationLabel();
    void onDeclinationDateLabelClicked();
    void updateUseGeoNorth(int b);
    void updateDeclinationDate(const QDate &d);
    void alignDeclinationDate(const QDate& d);
    void clearDataSelection_1();
    int clearDataSelection_2();
    int clearDataSelection_3();
    void dateRangeDetect();
    void clearPreviousDatapathEdit();
    void handleCrossWindAndAngleOfAttackUpdate(const QString& anem);

signals:
    void updateMetadataReadResult(bool b);
    void setDateRangeRequest(QPair<QDateTime, QDateTime>);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BasicSettingsPage::EmbeddedFileFlags)

#endif // BASICSETTINGSPAGE_H