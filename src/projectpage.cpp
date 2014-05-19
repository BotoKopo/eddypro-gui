/***************************************************************************
  projectpage.cpp
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

#include <QDebug>
#include <QStackedWidget>
#include <QGridLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QScrollArea>
#include <QUrl>
#include <QTimer>

#include <QwwButtonLineEdit/QwwButtonLineEdit>
#include <QwwClearLineEdit/QwwClearLineEdit>

#include "alia.h"
#include "fileutils.h"
#include "configstate.h"
#include "clicklabel.h"
#include "dbghelper.h"
#include "dlinidialog.h"
#include "dlproject.h"
#include "ecproject.h"
#include "dlrawfiledesctab.h"
#include "binarysettingsdialog.h"
#include "projectpage.h"
#include "splitter.h"
#include "mytabwidget.h"
#include "smartfluxbar.h"

ProjectPage::ProjectPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config) :
    QWidget(parent),
    metadataFileEdit(0),
    dynamicMdEdit(0),
    biomExtFileEdit(0),
    biomExtDirEdit(0),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config),
    helpGroup_(0),
    dlIniDialog_(0),
    faderWidget_(0),
    fadingOn_(true),
    isMetadataEditorOn_(false),
    previousFileType_(0),
    currentFileType_(0),
    previousMetadataFile_(QString()),
    currentMetadataFile_(QString()),
    binDialog_(0)
{
    DEBUG_FUNC_NAME

    titleLabel = new ClickLabel(tr("Project name :"));
    titleLabel->setProperty("optionalField", true);
    titleLabel->setToolTip(tr("<b>Project name:</b> Enter a name for the flux computation project. This will be the default file name for this project, but you can edit it while saving the project. This field is optional."));
    titleEdit = new QwwClearLineEdit();
    titleEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    titleEdit->setToolTip(titleLabel->toolTip());
    titleEdit->setMaxLength(255);

    fileTypeLabel = new ClickLabel();
    fileTypeLabel->setText(tr("Raw file format :"));
    fileTypeLabel->setToolTip(tr("<b>Raw file format:</b> Select the format of your raw files."
                                 "<br /><b><i>LI-COR GHG</i></b> is the format of files created by LI-COR data logging software."
                                 "<br /><b><i>ASCII plain text</i></b> includes virtually any text file organized in data columns. "
                                 "<br /><b><i>Generic Binary</i></b> is any fixed-length words binary format used to store single precision variables. Use the <b><i>Settings...</i></b> dialogue to further specify the format. "
                                 "<br /><b><i>TOB1</i></b> is a proprietary format by Campbell<sup>&reg;</sup> Scientific, used to collect raw files on its data loggers. "
                                 "<br />The two <b><i>SLT are binary</i></b> formats of raw files created by the EddySoft and EdiSol data collection tools."));

    ghgFileTyperRadio = new QRadioButton(tr("LI-COR GHG"));
    ghgFileTyperRadio->setChecked(true);
    ghgFileTyperRadio->setToolTip(tr("<b>LI-COR GHG:</b> A custom LI-COR raw file format. Each GHG file is an archive containing a data and a metadata file, both in readable text format. GHG files have extension \".ghg\""));
    asciiFileTypeRadio = new QRadioButton(tr("ASCII plain text"));
    asciiFileTypeRadio->setToolTip(tr("<b>ASCII plain text:</b> Any text file organized in data columns, with or without a header. All typical field separators (tab, space, comma, and semicolon) are supported. Campbell<sup>&reg;</sup> Scientific TOA5 format is an example of a supported ASCII data file."));
    tobFileTypeRadio = new QRadioButton(tr("TOB1"));
    tobFileTypeRadio->setToolTip(tr("<b>TOB1:</b> Raw files in the Campbell<sup>&reg;</sup> Scientific binary format. Support of TOB1 format is limited to files containing only ULONG and IEEE4 fields, or ULONG and FP2 fields. In the second case, FP2 fields must follow any ULONG field, while for ULONG and IEEE4 there is no such limitation. Note also that ULONG fields are not interpreted by EddyPro, thus they can only be defined as <i>Ignore</i> variables."));
    sltEddysoftFileTypeRadio = new QRadioButton(tr("SLT (EddySoft)"));
    sltEddysoftFileTypeRadio->setToolTip(tr("<b>SLT (EddySoft):</b> Format of binary files created by EddyMeas, the data acquisition tool of the EddySoft suite, by O. Kolle and C. Rebmann (Max Planck Institute, Jena, Germany). This is a fixed-length binary format. It includes a binary header in each file that needs to be interpreted to correctly retrieve data. EddyPro interprets such header automatically, but you still need to describe the file content in the Raw File Description tab of the Metadata File Editor."));
    sltEdisolFileTypeRadio = new QRadioButton(tr("SLT (EdiSol)"));
    sltEdisolFileTypeRadio->setToolTip(tr("<b>SLT (EdiSol):</b> Format of binary files created by EdiSol, the data acquisition tool developed by Univ. of Edinburg, UK. This is a fixed-length binary format. It includes a binary header in each file that needs to be interpreted to correctly retrieve data. EddyPro interprets such header automatically, but you still need to describe the file content in the Raw File Description tab of the Metadata File Editor."));
    binaryFileTypeRadio = new QRadioButton(tr("Generic binary"));
    binaryFileTypeRadio->setToolTip(tr("<b>Generic binary:</b> Generic binary (unformatted) raw data files. Limited to fixed-length binary words that contain data stored as single precision (real) numbers."));

    fileTypeRadioGroup = new QButtonGroup(this);
    fileTypeRadioGroup->addButton(ghgFileTyperRadio, 0);
    fileTypeRadioGroup->addButton(asciiFileTypeRadio, 1);
    fileTypeRadioGroup->addButton(tobFileTypeRadio, 2);
    fileTypeRadioGroup->addButton(sltEddysoftFileTypeRadio, 3);
    fileTypeRadioGroup->addButton(sltEdisolFileTypeRadio, 4);
    fileTypeRadioGroup->addButton(binaryFileTypeRadio, 5);

    tobSettingsCombo = new QComboBox(this);
    tobSettingsCombo->addItem(tr("Detect automatically"));
    tobSettingsCombo->addItem(tr("Only ULONG and IEEE4 fields"));
    tobSettingsCombo->addItem(tr("Only ULONG and FP2 fields"));
    tobSettingsCombo->setItemData(0, tr("<b>Detect automatically:</b> Let EddyPro figure out whether TOB1 files contain (ULONG and) IEEE4 fields or (ULONG and) FP2 fields."), Qt::ToolTipRole);
    tobSettingsCombo->setItemData(1, tr("<b>Only ULONG and IEEE4 fields:</b> Choose this option to specify that your TOB1 files contain only IEEE4 fields and possibly ULONG fields. Note that EddyPro does not interpret ULONG fields. This means that any variable stored in ULONG format must be marked with the <i>Ignore</i> option in the <b><i>Raw File Description</i></b> table. Typically ULONG format is used for timestamp information."), Qt::ToolTipRole);
    tobSettingsCombo->setItemData(2, tr("<b>Only ULONG and FP2 fields:</b> Choose this option to specify that your TOB1 files contain only FP2 fields and possibly ULONG fields. Note that ULONG fields, if present, must come first in the sequence of fields. Note also that EddyPro does not interpret ULONG fields. This means that any variable stored in ULONG format must be marked with the <i>Ignore</i> option in the <b><i>Raw File Description</i></b> table. Typically ULONG format is used for timestamp information."), Qt::ToolTipRole);
    tobSettingsCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    binSettingsButton = new QPushButton(tr("Settings..."));
    binSettingsButton->setProperty("commonButton", true);
    binSettingsButton->setMaximumWidth(binSettingsButton->sizeHint().width());
    binSettingsButton->setToolTip(tr("<b>Generic Binary Settings:</b> Use the <b><i>Settings...</i></b> button to provide specifications of the binary format, to help EddyPro to correctly read the files."));

    createMetadataEditor();

    createSlowMeasuresEditor();

    metadataEditors = new MyTabWidget;
    metadataEditors->setObjectName(QStringLiteral("metadataEditorsTabwidget"));
    metadataEditors->getTabBarAsPublic()->setObjectName(QStringLiteral("metadataEditorsTabbar"));
    metadataEditors->addTab(metadataTab, QStringLiteral("Metadata File Editor"));

    metadataLabel = new ClickLabel();
    metadataLabel->setText(tr("Metadata file :"));
    metadataLabel->setToolTip(tr("<b>Metadata:</b> Choose whether to use metadata files embedded into GHG files or to bypass them by using an alternative metadata file. Only applicable to raw files in LI-COR GHG format."));
    embMetadataFileRadio = new QRadioButton(tr("Use embedded file"));
    embMetadataFileRadio->setToolTip(tr("<b>Use embedded metadata file:</b> Select this option to use file-specific meta-information, retrieved from the metadata file residing inside the GHG archive."));
    altMetadataFileRadio = new QRadioButton(tr("Use alternative file:"));
    altMetadataFileRadio->setToolTip(tr("<b>Use alternative metadata file:</b> Select this option to use an alternative metadata file. Note that in this case all GHG files are processed using the same meta-information, retrieved from the alternative metadata file. This file is created and/or edited in the <b><i>Metadata File Editor</i></b>. If you are about to process GHG files, you can speed up the completion of the alternative METADATA by unzipping any raw file and loading the extracted METADATA from the Metadata file: Use alternative file <b><i>Load</i></b> button. Make changes if needed and save the file."));
    metadataRadioGroup = new QButtonGroup(this);
    metadataRadioGroup->addButton(embMetadataFileRadio, 0);
    metadataRadioGroup->addButton(altMetadataFileRadio, 1);

    metadataFileEdit = new QwwButtonLineEdit();
    metadataFileEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    metadataFileEdit->setButtonVisible(false);
    metadataFileEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    metadataFileEdit->setReadOnly(true);
    metadataFileEdit->installEventFilter(const_cast<ProjectPage*>(this));
    metadataFileEdit->setToolTip(tr("<b>Load:</b> Load an existing metadata file to edit it in the <i><b>Metadata File Editor</i></b>. If you use the <i><b>Metadata File Editor</i></b> to create and save a new metadata file from scratch, its path will appear here."));
    metadataFileEdit->setProperty("asButtonLineEdit", true);

    metadataFileLoad = new QPushButton(tr("Load..."));
    metadataFileLoad->setProperty("loadButton", true);
    metadataFileLoad->setToolTip(tr("<b>Load:</b> Load an existing metadata file to edit it in the <i><b>Metadata File Editor</i></b>. If you use the <i><b>Metadata File Editor</i></b> to create and save a new metadata file from scratch, its path will appear here."));

    QHBoxLayout* metadataContainerLayout = new QHBoxLayout;
    metadataContainerLayout->addWidget(metadataFileEdit);
    metadataContainerLayout->addWidget(metadataFileLoad);
    metadataContainerLayout->setStretch(2, 1);
    metadataContainerLayout->setContentsMargins(0, 0, 0, 0);
    metadataContainerLayout->setSpacing(0);
    QWidget* metadataFileContainer = new QWidget();
    metadataFileContainer->setLayout(metadataContainerLayout);

    createQuestionMark();

    dynamicMdCheckBox = new QCheckBox();
    dynamicMdCheckBox->setText(tr("Use dynamic metadata file :"));
    dynamicMdCheckBox->setToolTip(tr("<b>Use dynamic metadata file:</b> "
                                     "Check this option and provide the "
                                     "corresponding path to instruct "
                                     "EddyPro to use an externally-created "
                                     "file that contains time changing "
                                     "metadata, such as canopy height, "
                                     "instrument separations and more."));
    dynamicMdCheckBox->setProperty("optionalField", true);

    dynamicMdEdit = new QwwButtonLineEdit();
    dynamicMdEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    dynamicMdEdit->setButtonVisible(false);
    dynamicMdEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    dynamicMdEdit->setReadOnly(true);
    dynamicMdEdit->installEventFilter(const_cast<ProjectPage*>(this));
    dynamicMdEdit->setProperty("asButtonLineEdit", true);

    dynamicMdLoad = new QPushButton(tr("Load..."));
    dynamicMdLoad->setProperty("loadButton", true);
    dynamicMdLoad->setToolTip(tr("<b>Load:</b> Load an existing dynamic "
                                 "metadata file."));

    QHBoxLayout* dynamicMdLayout = new QHBoxLayout;
    dynamicMdLayout->addWidget(dynamicMdEdit);
    dynamicMdLayout->addWidget(dynamicMdLoad);
    dynamicMdLayout->setStretch(2, 1);
    dynamicMdLayout->setContentsMargins(0, 0, 0, 0);
    dynamicMdLayout->setSpacing(0);
    QWidget* dynamicMdContainer = new QWidget();
    dynamicMdContainer->setLayout(dynamicMdLayout);

    biomDataCheckBox = new QCheckBox();
    biomDataCheckBox->setText(tr("Biomet data :"));
    biomDataCheckBox->setProperty("optionalField", true);
    biomDataCheckBox->setToolTip(tr("<b>Biomet data:</b> Select this option and choose the source of biomet data. Biomet data are slow (1 Hz) measurements of biological and meteorological variables that complement eddy covariance measurements. Some biomet measurements can be used to improve flux results (ambient temperature, relative humidity and pressure, global radiation, PAR and long-wave incoming radiation). All biomet data available are screened for physical plausibility, averaged on the same time scale of the fluxes, and provided in a separate output file."));

    biomEmbFileRadio = new QRadioButton(tr("Use embedded files "));
    biomEmbFileRadio->setChecked(true);
    biomEmbFileRadio->setToolTip(tr("<b>Use embedded files:</b> Choose this option to use data from biomet files embedded in the LI-COR GHG files. This option is only available for GHG files collected with the LI-7550 embedded software version 6.0.0 or newer, provided a biomet system was used during data collection. EddyPro will automatically read biomet files from the GHG bundles, interpret them and extract relevant variables."));

    biomExtFileRadio = new QRadioButton(tr("Use external file:"));
    biomExtFileRadio->setToolTip(tr("<b>Use external file:</b> Select this option if you have all biomet data collected in one only external file, and provide the path to this file by using the <b><i>Load...</i></b> button. <br /><b>IMPORTANT:</b> The biomet file must be formatted according the guidelines that you can find in EddyPro Help and User\'s Guide. Click on the question mark at the right side of the <b><i>Load...</i></b> button to access the guidelines on EddyPro Help."));
    biomExtFileEdit = new QwwButtonLineEdit();
    biomExtFileEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    biomExtFileEdit->setButtonVisible(false);
    biomExtFileEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    biomExtFileEdit->setReadOnly(true);
    biomExtFileEdit->installEventFilter(const_cast<ProjectPage*>(this));
    biomExtFileEdit->setProperty("asButtonLineEdit", true);

    biomExtFileLoad = new QPushButton(tr("Load..."));
    biomExtFileLoad->setProperty("loadButton", true);
    biomExtFileLoad->setToolTip(tr("<b>Load:</b> Load an existing biomet external file."));

    QHBoxLayout* biomExtContainerLayout = new QHBoxLayout;
    biomExtContainerLayout->addWidget(biomExtFileEdit);
    biomExtContainerLayout->addWidget(biomExtFileLoad);
    biomExtContainerLayout->setStretch(2, 1);
    biomExtContainerLayout->setContentsMargins(0, 0, 0, 0);
    biomExtContainerLayout->setSpacing(0);
    QWidget* biomExtContainer = new QWidget();
    biomExtContainer->setLayout(biomExtContainerLayout);

    biomExtDirRadio = new QRadioButton(tr("Use external directory:"));
    biomExtDirRadio->setToolTip(tr("<b>Use external directory:</b> Select this option if you have biomet data collected in more than one external file, and provide the path to directory that contains those files by using the <b><i>Browse...</i></b> button. <br /><b>IMPORTANT:</b> All biomet files must be formatted according the guidelines that you can find in EddyPro Help and User\'s Guide. Click on the question mark at the right side of the <b><i>Browse...</i></b> button to access the guidelines page on EddyPro Help."));

    biomExtDirEdit = new QwwButtonLineEdit();
    biomExtDirEdit->setReadOnly(true);
    biomExtDirEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    biomExtDirEdit->setButtonVisible(false);
    biomExtDirEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    biomExtDirEdit->setProperty("asButtonLineEdit", true);
    biomExtDirEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    biomExtDirEdit->installEventFilter(const_cast<ProjectPage*>(this));
    biomExtDirBrowse = new QPushButton(tr("Browse..."));
    biomExtDirBrowse->setProperty("loadButton", true);
    biomExtDirBrowse->setToolTip(tr("<b>Browse :</b> Use to specify the folder that contains the external biomet data. If data are also contained in subfolders, select the <i>Search in subfolders</i> box."));

    QHBoxLayout* biomExtDirContainerLayout = new QHBoxLayout;
    biomExtDirContainerLayout->addWidget(biomExtDirEdit);
    biomExtDirContainerLayout->addWidget(biomExtDirBrowse);
    biomExtDirContainerLayout->setStretch(2, 1);
    biomExtDirContainerLayout->setContentsMargins(0, 0, 0, 0);
    biomExtDirContainerLayout->setSpacing(0);
    QWidget* biomExtDirContainer = new QWidget();
    biomExtDirContainer->setLayout(biomExtDirContainerLayout);

    biomExtDirRecCheckBox = new QCheckBox();
    biomExtDirRecCheckBox->setText(tr("Search in subfolders"));
    biomExtDirRecCheckBox->setToolTip(tr("<b>Search in subfolders:</b> Check this box if biomet data are in subfolders in the selected directory."));

    biomExtDirSuffixLabel = new ClickLabel(tr("Files extension :"), this);
    biomExtDirSuffixLabel->setToolTip(tr("<b>Files extension :</b> Select or directly edit the extension of the biomet files in the selected folder. We recommend removing any other file with the same extension from this folder (and possibly its subfolders) to avoid conflicts."));
    biomExtDirCombo = new QComboBox;
    biomExtDirCombo->setEditable(true);
    biomExtDirCombo->addItems(QStringList()
                              << QStringLiteral("txt")
                              << QStringLiteral("dat")
                              << QStringLiteral("csv")
                              << QStringLiteral("met")
                              << QStringLiteral("meteo"));
    biomExtDirCombo->setStyleSheet(QStringLiteral("QComboBox {min-width: 72px;"
                                                 "max-width: 72px;}"));
    biomExtDirCombo->setToolTip(biomExtDirSuffixLabel->toolTip());

    biomRadioGroup = new QButtonGroup(this);
    biomRadioGroup->addButton(biomEmbFileRadio, 0);
    biomRadioGroup->addButton(biomExtFileRadio, 1);
    biomRadioGroup->addButton(biomExtDirRadio, 2);

    QHBoxLayout *altMetadataFileRadioBox = new QHBoxLayout;
    altMetadataFileRadioBox->addWidget(altMetadataFileRadio);
    altMetadataFileRadioBox->addWidget(questionMark_4);
    altMetadataFileRadioBox->addStretch();

    QGridLayout *gridLeft = new QGridLayout();
    gridLeft->addWidget(titleLabel, 0, 0, Qt::AlignRight);
    gridLeft->addWidget(titleEdit, 0, 1, 1, 3);
    gridLeft->addWidget(fileTypeLabel, 1, 0, Qt::AlignRight);
    gridLeft->addWidget(ghgFileTyperRadio, 1, 1, Qt::AlignLeft);
    gridLeft->addWidget(questionMark_2, 1, 2, Qt::AlignLeft);
    gridLeft->addWidget(asciiFileTypeRadio, 2, 1, Qt::AlignLeft);
    gridLeft->addWidget(binaryFileTypeRadio, 3, 1, Qt::AlignLeft);
    gridLeft->addWidget(questionMark_6, 3, 2, 1, 1, Qt::AlignLeft);
    gridLeft->addWidget(binSettingsButton, 3, 3);
    gridLeft->addWidget(tobFileTypeRadio, 4, 1, Qt::AlignLeft);
    gridLeft->addWidget(questionMark_5, 4, 2, 1, 1, Qt::AlignLeft);
    gridLeft->addWidget(tobSettingsCombo, 4, 3);
    gridLeft->addWidget(sltEddysoftFileTypeRadio, 5, 1, Qt::AlignLeft);
    gridLeft->addWidget(sltEdisolFileTypeRadio, 6, 1, Qt::AlignLeft);
    gridLeft->setRowMinimumHeight(1, 21);
    gridLeft->setRowMinimumHeight(2, 21);
    gridLeft->setRowMinimumHeight(3, 21);
    gridLeft->setRowMinimumHeight(4, 21);
    gridLeft->setRowMinimumHeight(5, 21);
    gridLeft->setRowMinimumHeight(6, 31);
    gridLeft->setRowStretch(7, 1);
    gridLeft->setColumnStretch(4, 1);
    gridLeft->setVerticalSpacing(3);

    QGridLayout *gridRight = new QGridLayout;
    gridRight->addWidget(metadataLabel, 1, 0, Qt::AlignRight);
    gridRight->addWidget(embMetadataFileRadio, 1, 1, 1, 1, Qt::AlignLeft);
    gridRight->addLayout(altMetadataFileRadioBox, 2, 1);
    gridRight->addWidget(metadataFileContainer, 3, 1, 1, 3);
    gridRight->addWidget(dynamicMdCheckBox, 5, 0, Qt::AlignRight);
    gridRight->addWidget(dynamicMdContainer, 5, 1, 1, 3);
    gridRight->addWidget(questionMark_3, 5, 4);
    gridRight->addWidget(biomDataCheckBox, 7, 0, Qt::AlignRight);
    gridRight->addWidget(biomEmbFileRadio, 7, 1, 1, 1, Qt::AlignLeft);
    gridRight->addWidget(biomExtFileRadio, 8, 1, 1, 1, Qt::AlignLeft);
    gridRight->addWidget(biomExtContainer, 8, 2, 1, 2);
    gridRight->addWidget(questionMark_7, 8, 4);
    gridRight->addWidget(biomExtDirRadio, 9, 1, 1, 1, Qt::AlignLeft);
    gridRight->addWidget(biomExtDirContainer, 9, 2, 1, 2);
    gridRight->addWidget(questionMark_8, 9, 4);
    gridRight->addWidget(biomExtDirRecCheckBox, 10, 1, Qt::AlignRight);
    gridRight->addWidget(biomExtDirSuffixLabel, 10, 2, Qt::AlignRight);
    gridRight->addWidget(biomExtDirCombo, 10, 3);
    gridRight->setRowMinimumHeight(4, 44);
    gridRight->setRowStretch(11, 1);
    gridRight->setRowMinimumHeight(1, 21);
    gridRight->setRowMinimumHeight(2, 21);
    gridRight->setRowMinimumHeight(3, 21);
    gridRight->setRowMinimumHeight(4, 21);
    gridRight->setRowMinimumHeight(5, 21);
    gridRight->setRowMinimumHeight(6, 21);
    gridRight->setRowMinimumHeight(7, 21);
    gridRight->setRowMinimumHeight(8, 21);
    gridRight->setRowMinimumHeight(9, 21);
    gridRight->setRowMinimumHeight(10, 21);
    gridRight->setVerticalSpacing(3);
    gridRight->setColumnStretch(2, 1);
    gridRight->setContentsMargins(0, 0, 50, 0);

    QGridLayout *hBox = new QGridLayout;
    hBox->addLayout(gridLeft, 0, 0);
    hBox->addLayout(gridRight, 0, 1);
    hBox->setColumnStretch(0, 1);
    hBox->setColumnStretch(1, 1);

    QWidget* upAreaWidget = new QWidget;
    upAreaWidget->setLayout(hBox);
    upAreaWidget->setProperty("scrollContainerWidget", true);

    QScrollArea* upScrollArea = new QScrollArea;
    upScrollArea->setWidget(upAreaWidget);
    upScrollArea->setWidgetResizable(true);

    QLabel* upGroupTitle = new QLabel(tr("Project Info"));
    upGroupTitle->setProperty("groupTitle", true);

    Splitter *splitter = new Splitter(Qt::Vertical, this);
    splitter->addWidget(upScrollArea);
    splitter->addWidget(metadataEditors);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->handle(1)->setToolTip(tr("Handle the separator."));
    splitter->setContentsMargins(15, 15, 15, 10);

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);
    smartfluxBar_->setVisible(false);
    smartfluxBar_->setToolTip(tr("EddyPro is in SMARTFlux configuration mode (Ctrl+F to exit)"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(smartfluxBar_);
    mainLayout->addWidget(upGroupTitle);
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    connect(ecProject_, SIGNAL(ecProjectNew()),
            this, SLOT(reset()));
    connect(ecProject_, SIGNAL(ecProjectChanged()),
            this, SLOT(refresh()));

    connect(dlIniDialog_, SIGNAL(metadataFileSaved(QString)),
            this, SLOT(updateMetadataFileEdit(QString)));
    connect(dlIniDialog_, SIGNAL(mdFileEditClearRequest()),
            metadataFileEdit, SLOT(clear()));

    connect(titleLabel, SIGNAL(clicked()),
            this, SLOT(onTitleLabelClicked()));
    connect(titleEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateTitle(QString)));

    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateFileType(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fileTypeRadioClicked_1(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fileTypeRadioClicked_2(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateUseMetadataFile_1(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fadeInWidget(int)));

    connect(tobSettingsCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(tobSettingsUpdate(int)));
    connect(tobSettingsCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTooltip(int)));

    connect(binSettingsButton, SIGNAL(clicked()),
            this, SLOT(binSettingsDialog()));

    connect(metadataRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(metadataRadioClicked(int)));
    connect(metadataRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateUseMetadataFile_2(int)));

    connect(metadataFileEdit, SIGNAL(buttonClicked()),
            this, SLOT(mdResetRequest()));
    connect(metadataFileEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateMetadataFile(QString)));
    connect(metadataFileLoad, SIGNAL(clicked()),
            this, SLOT(metadataFileLoad_clicked()));

    connect(dynamicMdCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(onTimelineFileCheckBoxClicked(bool)));
    connect(dynamicMdCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateUseTimelineFile(bool)));
    connect(dynamicMdEdit, SIGNAL(buttonClicked()),
            this, SLOT(clearDynamicMdEdit()));
    connect(dynamicMdEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateTimelineFile(const QString &)));
    connect(dynamicMdLoad, SIGNAL(clicked()),
            this, SLOT(timelineFileLoad_clicked()));

    connect(biomDataCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(on_biomDataCheckBox_clicked(bool)));
    connect(biomRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(on_biomRadioGroup_clicked_1(int)));
    connect(biomRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(on_biomRadioGroup_clicked_2(int)));

    connect(biomExtFileEdit, SIGNAL(buttonClicked()),
            this, SLOT(clearBiomExtFileEdit()));
    connect(biomExtFileEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateBiomFile(QString)));
    connect(biomExtFileLoad, SIGNAL(clicked()),
            this, SLOT(on_biomFileLoad_clicked()));

    connect(biomExtDirEdit, SIGNAL(buttonClicked()),
            this, SLOT(clearBiomExtDirEdit()));
    connect(biomExtDirEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateBiomDir(QString)));
    connect(biomExtDirBrowse, SIGNAL(clicked()),
            this, SLOT(on_biomDirBrowse_clicked()));

    connect(biomExtDirRecCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateExtDirRec(bool)));

    connect(biomExtDirSuffixLabel, SIGNAL(clicked()),
            this, SLOT(onBiomExtDirSuffixLabelClicked()));
    connect(biomExtDirCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateExtDirSuffix(QString)));
    connect(biomExtDirCombo, SIGNAL(editTextChanged(QString)),
            this, SLOT(updateExtDirSuffix(QString)));

    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));

    QTimer::singleShot(0, this, SLOT(reset()));
}

ProjectPage::~ProjectPage()
{
    DEBUG_FUNC_NAME
    if (binDialog_)
        delete binDialog_;
}

// modeless dialog
void ProjectPage::createMetadataEditor()
{
    QLabel* helpLabel = new QLabel;
    helpLabel->setText(tr("This dialog will activate if you: <br /><ul><li>select a "
                          "Raw file format different than LI-COR GHG</li><br />"
                          "<li>select Metadata file: Use alternative file</li></ul>"));
    helpLabel->setObjectName(QStringLiteral("helpLabel"));

    QVBoxLayout *helpLayout = new QVBoxLayout;
    helpLayout->addWidget(helpLabel);
    helpLayout->addStretch();

    helpGroup_ = new QFrame();
    helpGroup_->setLayout(helpLayout);

    dlIniDialog_ = new DlIniDialog(this, dlProject_, configState_);

    metadataTab = new QStackedWidget();
    metadataTab->addWidget(helpGroup_);
    metadataTab->addWidget(dlIniDialog_);
    metadataTab->setCurrentIndex(0);
    qDebug() << "metadataTab->setCurrentIndex(0)";
}

// NOTE: not used
void ProjectPage::createSlowMeasuresEditor()
{
}

void ProjectPage::fadeInWidget(int filetype)
{
    DEBUG_FUNC_NAME

    if (fadingOn_ && (filetype != previousFileType_))
    {
        if (faderWidget_)
            faderWidget_->close();

        if (filetype == Defs::RawFileTypeGHG)
        {
            if (!isMetadataEditorOn_)
            {
                if (!faderWidget_)
                {
                    faderWidget_ = new FaderWidget(helpGroup_);
                    faderWidget_->setFadeDuration(200);
                }
                faderWidget_->start();
            }
        }
        else if (previousFileType_ == Defs::RawFileTypeGHG)
        {
            if (!faderWidget_)
            {
                faderWidget_ = new FaderWidget(dlIniDialog_);
                faderWidget_->setFadeDuration(200);
            }
            faderWidget_->start();
        }
    }
}

void ProjectPage::metadataFileLoad_clicked()
{
    DEBUG_FUNC_NAME

    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString mdFile = QFileDialog::getOpenFileName(this,
                    tr("Select the Metadata File"),
                    searchPath,
                    tr("%1 Metadata Files (*.metadata);;All Files (*.*)").arg(Defs::APP_NAME),
                    0
                    );

    if (!mdFile.isEmpty())
    {
        QFileInfo mdDir(mdFile);
        QString mdPath = mdDir.canonicalPath();

        configState_->window.last_data_path = mdPath;
        Alia::updateLastDatapath(mdPath);

        bool embedded = false;
        if (dlIniDialog_->openFile(mdFile, embedded))
        {
            updateMetadataFileEdit(mdFile);
            altMetadataFileRadio->setChecked(true);
            isMetadataEditorOn_ = true;
            metadataTab->setCurrentIndex(1);
            qDebug() << "metadataTab->setCurrentIndex(1)";
            fadeInWidget(1);
        }
    }
}

void ProjectPage::on_biomFileLoad_clicked()
{
    DEBUG_FUNC_NAME

    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString mdFile = QFileDialog::getOpenFileName(this,
                    tr("Select the Biomet File"),
                    searchPath,
                    tr("%1 Biomet Files (*.csv);;All Files (*.*)").arg(Defs::APP_NAME),
                    0
                    );

    if (!mdFile.isEmpty())
    {
        QFileInfo mdDir(mdFile);
        QString mdPath = mdDir.canonicalPath();

        configState_->window.last_data_path = mdPath;
        Alia::updateLastDatapath(mdPath);

        updateBiomFileEdit(mdFile);
    }
}

void ProjectPage::onTitleLabelClicked()
{
    titleEdit->setFocus();
    titleEdit->selectAll();
}

void ProjectPage::updateMetadataFileEdit(const QString &filename)
{
    DEBUG_FUNC_NAME
    QFileInfo filePath(filename);
    QString canonicalFilePath = filePath.canonicalFilePath();
    metadataFileEdit->setText(QDir::toNativeSeparators(canonicalFilePath));
    updateMetadataLoading();
}

void ProjectPage::updateBiomFileEdit(const QString &filename)
{
    DEBUG_FUNC_NAME
    QFileInfo filePath(filename);
    QString canonicalFilePath = filePath.canonicalFilePath();
    biomExtFileEdit->setText(QDir::toNativeSeparators(canonicalFilePath));
    updateMetadataLoading();
}

void ProjectPage::updateTitle(const QString& t)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralTitle(t);
}

void ProjectPage::updateFileType(int filetype)
{
    ecProject_->setGeneralFileType(filetype);
}

void ProjectPage::updateUseMetadataFile_1(int filetype)
{
    DEBUG_FUNC_NAME
    if (filetype == Defs::RawFileTypeGHG)
    {
        ecProject_->setGeneralUseAltMdFile(altMetadataFileRadio->isChecked());
    }
    else
    {
        ecProject_->setGeneralUseAltMdFile(true);
    }
    updateMetadataLoading();
}

void ProjectPage::updateUseMetadataFile_2(int radio)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralUseAltMdFile(radio);
    updateMetadataLoading();
}

void ProjectPage::updateMetadataFile(const QString& fp)
{
    qDebug() << "previousMetadataFile_" << previousMetadataFile_;
    qDebug() << "currentMetadataFile_" << currentMetadataFile_;

    previousMetadataFile_ = currentMetadataFile_;
    currentMetadataFile_ = QDir::cleanPath(fp);

    qDebug() << "previousMetadataFile_" << previousMetadataFile_;
    qDebug() << "currentMetadataFile_" << currentMetadataFile_;

    ecProject_->setGeneralMdFilepath(currentMetadataFile_);
    metadataFileEdit->setButtonVisible(metadataFileEdit->isEnabled()
                                       && !metadataFileEdit->text().isEmpty());
    Alia::updateLineEditToolip(metadataFileEdit);
}

void ProjectPage::updateBiomFile(const QString& fp)
{
    ecProject_->setGeneralBiomFile(QDir::cleanPath(fp));
    biomExtFileEdit->setButtonVisible(biomExtFileEdit->isEnabled()
                                      && !biomExtFileEdit->text().isEmpty());
    Alia::updateLineEditToolip(biomExtFileEdit);
}

void ProjectPage::updateBiomDir(const QString& fp)
{
    ecProject_->setGeneralBiomDir(QDir::cleanPath(fp));
    biomExtDirEdit->setButtonVisible(biomExtDirEdit->isEnabled()
                                     && !biomExtDirEdit->text().isEmpty());
    Alia::updateLineEditToolip(biomExtDirEdit);
}

void ProjectPage::onBiomExtDirSuffixLabelClicked()
{
    biomExtDirCombo->setFocus();
    biomExtDirCombo->showPopup();
}

void ProjectPage::reset()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    qDebug() << "1";
    titleEdit->clear();
    qDebug() << "2";
    fileTypeRadioGroup->buttons().at(0)->setChecked(true);
    qDebug() << "3";
    binSettingsButton->setEnabled(false);
    qDebug() << "4";
    tobSettingsCombo->setEnabled(false);
    qDebug() << "5";
    Alia::resetComboToItem(tobSettingsCombo, 0);
    qDebug() << "6";

    embMetadataFileRadio->setEnabled(true);
    embMetadataFileRadio->setChecked(true);
    altMetadataFileRadio->setChecked(false);
    metadataRadioClicked(0);

    mdReset();
    metadataFileEdit->setEnabled(false);

    dynamicMdEdit->clear();
    dynamicMdEdit->setEnabled(false);
    dynamicMdCheckBox->setChecked(false);
    dynamicMdLoad->setEnabled(false);

    biomDataCheckBox->setChecked(false);
    biomEmbFileRadio->setChecked(true);
    biomExtFileEdit->clear();
    biomExtDirEdit->clear();
    biomExtDirRecCheckBox->setChecked(false);
    biomExtDirCombo->setCurrentIndex(0);
    on_biomDataCheckBox_clicked(false);

    refreshMetadata();
    updateMetadataLoading();

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void ProjectPage::refresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    if (titleEdit->text() != ecProject_->generalTitle())
        titleEdit->setText(ecProject_->generalTitle());

    fileTypeRadioGroup->buttons().at(ecProject_->generalFileType())->setChecked(true);
    fileTypeRadioClicked_2(ecProject_->generalFileType());

    tobSettingsCombo->setCurrentIndex(ecProject_->generalTob1Format());

    embMetadataFileRadio->setChecked(!ecProject_->generalUseAltMdFile());
    altMetadataFileRadio->setChecked(ecProject_->generalUseAltMdFile());
    metadataRadioClicked(ecProject_->generalUseAltMdFile());

    refreshMetadata();
    Alia::updateLineEditToolip(metadataFileEdit);

    dynamicMdCheckBox->setChecked(ecProject_->generalUseTimelineFile());
    dynamicMdEdit->setText(QDir::toNativeSeparators(ecProject_->generalTimelineFilepath()));
    Alia::updateLineEditToolip(dynamicMdEdit);
    dynamicMdEdit->setEnabled(dynamicMdCheckBox->isChecked());
    dynamicMdLoad->setEnabled(dynamicMdCheckBox->isChecked());

    int useBiom = ecProject_->generalUseBiomet();
    biomDataCheckBox->setChecked(useBiom);
    if (useBiom)
    {
        biomRadioGroup->buttons().at(useBiom - 1)->setChecked(true);
    }
    else
    {
        if ((ecProject_->generalFileType() == Defs::RawFileTypeGHG))
        {
            biomEmbFileRadio->setChecked(true);
        }
        else
        {
            biomExtFileRadio->click();
        }
    }
    biomExtFileEdit->setText(QDir::toNativeSeparators(ecProject_->generalBiomFile()));
    Alia::updateLineEditToolip(biomExtFileEdit);
    biomExtDirEdit->setText(QDir::toNativeSeparators(ecProject_->generalBiomDir()));
    Alia::updateLineEditToolip(biomExtDirEdit);
    biomExtDirRecCheckBox->setChecked(ecProject_->generalBiomRecurse());
    QString s(ecProject_->generalBiomExt());
    if (!s.isEmpty())
    {
        int n = biomExtDirCombo->findText(s);
        if (n < 0)
        {
            biomExtDirCombo->addItem(s);
            biomExtDirCombo->setCurrentIndex(biomExtDirCombo->findText(s));
        }
        else
            biomExtDirCombo->setCurrentIndex(n);
    }
    else
    {
        biomExtDirCombo->setCurrentIndex(0);
    }
    on_biomDataCheckBox_clicked(biomDataCheckBox->isChecked());

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void ProjectPage::refreshMetadata()
{
    DEBUG_FUNC_NAME

    QString mdFile(ecProject_->generalMdFilepath());
    if (mdFile != metadataFileEdit->text())
    {
        if (!mdFile.isEmpty())
        {
            qDebug() << "ecProject_->generalUseMetadataFile()" << ecProject_->generalUseAltMdFile();
            if (QFile::exists(mdFile))
            {
                bool embedded = !ecProject_->generalUseAltMdFile();
                if (dlIniDialog_->openFile(mdFile, embedded))
                {
                    updateMetadataFileEdit(mdFile);
                }
                // silently discard
                else
                {
                    mdReset();
                }
            }
        }
        // silently discard
        else
        {
            mdReset();
        }
    }

    if (ecProject_->generalUseAltMdFile())
    {
        isMetadataEditorOn_ = true;
        metadataTab->setCurrentIndex(1);
        qDebug() << "metadataTab->setCurrentIndex(1)";
    }
    else
    {
        isMetadataEditorOn_ = false;
        metadataTab->setCurrentIndex(0);
        qDebug() << "metadataTab->setCurrentIndex(0)";
    }
}

void ProjectPage::updateMetadataLoading()
{
    DEBUG_FUNC_NAME

    updateMetadataFile(metadataFileEdit->text());
    if (!currentMetadataFile_.isEmpty())
    {
        if (QFile::exists(currentMetadataFile_))
        {
            emit updateMetadataReadRequest();
        }
        emit updateRawFilenameFormatRequest();
    }
}

void ProjectPage::fileTypeRadioClicked_1(int fileType)
{
    DEBUG_FUNC_NAME
    if (fileType == Defs::RawFileTypeGHG)
    {
        if (ecProject_->generalMdFilepath().isEmpty())
        {
            isMetadataEditorOn_ = false;
            metadataTab->setCurrentIndex(0);
        }
        else
        {
            isMetadataEditorOn_ = true;
            metadataTab->setCurrentIndex(1);
        }
    }
    else
    {
        isMetadataEditorOn_ = true;
        metadataTab->setCurrentIndex(1);
    }

    previousFileType_ = currentFileType_;
    currentFileType_ = fileType;
}

void ProjectPage::fileTypeRadioClicked_2(int fileType)
{
    DEBUG_FUNC_NAME

    // if licor
    if (fileType == Defs::RawFileTypeGHG)
    {
        embMetadataFileRadio->setEnabled(true);
        embMetadataFileRadio->setChecked(metadataFileEdit->text().isEmpty());
        altMetadataFileRadio->setChecked(!(metadataFileEdit->text()).isEmpty());
        metadataFileEdit->setEnabled(altMetadataFileRadio->isChecked());
        metadataFileLoad->setEnabled(altMetadataFileRadio->isChecked());

        biomEmbFileRadio->setEnabled(biomDataCheckBox->isChecked());
    }
    else
    {
        embMetadataFileRadio->setChecked(false);
        embMetadataFileRadio->setEnabled(false);
        altMetadataFileRadio->setChecked(true);
        metadataFileEdit->setEnabled(true);
        metadataFileLoad->setEnabled(true);

        biomEmbFileRadio->setEnabled(false);
        if (biomEmbFileRadio->isChecked())
        {
            biomExtFileRadio->click();
        }
    }

    tobSettingsCombo->setEnabled(fileType == Defs::RawFileTypeTOB1);

    binSettingsButton->setEnabled(fileType == Defs::RawFileTypeBIN);

    if (fileType == Defs::RawFileTypeSLT1 || fileType == Defs::RawFileTypeSLT2)
    {
        dlIniDialog_->rawFileDescTab()->rawSettingsButton->setEnabled(false);
    }
    else
    {
        dlIniDialog_->rawFileDescTab()->rawSettingsButton->setEnabled(true);
    }
}

void ProjectPage::loginiFileNew_clicked()
{
    DEBUG_FUNC_NAME
}

void ProjectPage::metadataRadioClicked(int b)
{
    DEBUG_FUNC_NAME

    isMetadataEditorOn_ = b;
    metadataTab->setCurrentIndex(b);

    // licor file type
    if (ecProject_->generalFileType() == Defs::RawFileTypeGHG)
    {
        // with or without alternative metadata
        metadataFileEdit->setEnabled(b);
        metadataFileLoad->setEnabled(b);
    }
    else
    {
        metadataFileEdit->setEnabled(true);
        metadataFileLoad->setEnabled(true);
    }
}

void ProjectPage::createQuestionMark()
{
    questionMark_2 = new QPushButton;
    questionMark_2->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_3 = new QPushButton;
    questionMark_3->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_4 = new QPushButton;
    questionMark_4->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_5 = new QPushButton;
    questionMark_5->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_6 = new QPushButton;
    questionMark_6->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_7 = new QPushButton;
    questionMark_7->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_8 = new QPushButton;
    questionMark_8->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_2, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_2()));
    connect(questionMark_3, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_3()));
    connect(questionMark_4, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_4()));
    connect(questionMark_5, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_5()));
    connect(questionMark_6, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_6()));
    connect(questionMark_7, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_7()));
    connect(questionMark_8, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_8()));
}

// used to avoid loading projects with no ghg or with
// alternative metadata which could block the basic settings page.
// moreover, set the biomet embedded case
void ProjectPage::setSmartfluxUI()
{
    DEBUG_FUNC_NAME
    bool on = configState_->project.smartfluxMode;

    qDebug() << "on" << on;

    // block project modified() signal
    bool oldmod;
    if (!on)
    {
        // save the modified flag to prevent side effects of setting widgets
        oldmod = ecProject_->modified();
        ecProject_->blockSignals(true);
    }

    if (on)
    {
        fileTypeRadioGroup->button(0)->click();
        metadataRadioGroup->button(0)->click();

        dynamicMdCheckBox->setChecked(false);
        dynamicMdEdit->clear();

        biomDataCheckBox->setChecked(true);
        biomRadioGroup->button(0)->click();
    }

    // restore project modified() signal
    if (!on)
    {
        // restore modified flag
        ecProject_->setModified(oldmod);
        ecProject_->blockSignals(false);
    }

    qDebug() << "ecProject_->generalUseBiomet()" << ecProject_->generalUseBiomet();
}

void ProjectPage::onlineHelpTrigger_2()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#LI-COR_GHG_File_Format.htm")));
}

void ProjectPage::onlineHelpTrigger_3()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Dynamic_Metadata.htm")));
}

void ProjectPage::onlineHelpTrigger_4()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Use_Alternative_Metadata_File.htm")));
}

void ProjectPage::onlineHelpTrigger_5()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#TOB1_Files.htm")));
}

void ProjectPage::onlineHelpTrigger_6()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Generic_Binary_Files.htm")));
}

void ProjectPage::onlineHelpTrigger_7()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Biomet_Data_Format.htm#ExternalBiomet")));
}

void ProjectPage::onlineHelpTrigger_8()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Biomet_Data_Format.htm#ExternalBiomet")));
}

void ProjectPage::binSettingsDialog()
{
    if (!binDialog_)
    {
        qDebug() << "create dialog";
        binDialog_ = new BinarySettingsDialog(this, ecProject_);
        emit connectBinarySettingsRequest();
    }
    binDialog_->refresh();

    binDialog_->show();
    binDialog_->raise();
    binDialog_->activateWindow();
}

bool ProjectPage::eventFilter(QObject* watched, QEvent* event)
{
    QList<QwwButtonLineEdit*> buttonList;

    if (metadataFileEdit)
        buttonList << metadataFileEdit;
    if (dynamicMdEdit)
        buttonList << dynamicMdEdit;
    if (biomExtFileEdit)
        buttonList << biomExtFileEdit;
    if (biomExtDirEdit)
        buttonList << biomExtDirEdit;

    if (!buttonList.isEmpty())
    {
        foreach (QwwButtonLineEdit* lineEdit,
                 buttonList)
        {
            QwwButtonLineEdit *watchedLineEdit = static_cast<QwwButtonLineEdit*>(watched);
            if (lineEdit && watchedLineEdit)
            {
                if (lineEdit == watchedLineEdit && event->type() == QEvent::EnabledChange)
                {
                    lineEdit->setButtonVisible(lineEdit->isEnabled()
                                               && !lineEdit->text().isEmpty());
                }
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

void ProjectPage::tobSettingsUpdate(int n)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralTob1Format(n);
}

void ProjectPage::onTimelineFileCheckBoxClicked(bool b)
{
    dynamicMdEdit->setEnabled(b);
    dynamicMdLoad->setEnabled(b);
}

void ProjectPage::updateUseTimelineFile(bool b)
{
    ecProject_->setGeneralUseTimelineFile(b);
    dynamicMdEdit->setButtonVisible(dynamicMdEdit->isEnabled() && !dynamicMdEdit->text().isEmpty());
}

void ProjectPage::updateTimelineFile(const QString& fp)
{
    ecProject_->setGeneralTimelineFilepath(QDir::cleanPath(fp));
    dynamicMdEdit->setButtonVisible(dynamicMdEdit->isEnabled() && !dynamicMdEdit->text().isEmpty());
    Alia::updateLineEditToolip(dynamicMdEdit);
}

void ProjectPage::timelineFileLoad_clicked()
{
    DEBUG_FUNC_NAME

    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString paramFile = QFileDialog::getOpenFileName(this,
                    tr("Select the Dynamic Metadata File"),
                    searchPath,
                    tr("All Files (*.*)")
                    );

    if (!paramFile.isEmpty())
    {
        QFileInfo paramFilePath(paramFile);
        QString canonicalParamFile = paramFilePath.canonicalFilePath();
        QString lastPath = paramFilePath.canonicalPath();

        configState_->window.last_data_path = lastPath;
        Alia::updateLastDatapath(lastPath);

        dynamicMdEdit->setText(QDir::toNativeSeparators(canonicalParamFile));
    }
}

void ProjectPage::mdResetRequest()
{
    if (Alia::queryMdReset() == QMessageBox::Yes)
    {
        mdReset();
        Alia::updateLineEditToolip(metadataFileEdit);
        emit requestBasicSettingsClear();
    }
}

void ProjectPage::mdReset()
{
    dlIniDialog_->defaults_2();
}

void ProjectPage::updateTooltip(int i)
{
    DEBUG_FUNC_NAME
    QComboBox* senderCombo = qobject_cast<QComboBox *>(sender());

    Alia::updateComboItemTooltip(senderCombo, i);
}

void ProjectPage::on_biomDataCheckBox_clicked(bool clicked)
{
    DEBUG_FUNC_NAME
    foreach (QWidget *w,
             QList<QWidget *>() << biomExtFileRadio
                                << biomExtDirRadio)
    {
        w->setEnabled(clicked);
    }

    if (clicked)
    {
        ecProject_->setGeneralUseBiomet(biomRadioGroup->checkedId() + 1);

        biomEmbFileRadio->setEnabled(ecProject_->generalFileType() == Defs::RawFileTypeGHG);
        if (ecProject_->generalFileType() != Defs::RawFileTypeGHG)
        {
            if (ecProject_->generalUseBiomet() == 1)
            {
                biomExtFileRadio->click();
            }
        }
        emit setOutputBiometRequest();
    }
    else
    {
        ecProject_->setGeneralUseBiomet(0);
        biomEmbFileRadio->setEnabled(false);
    }

    biomExtFileEdit->setEnabled(ecProject_->generalUseBiomet() == 2);
    biomExtFileLoad->setEnabled(ecProject_->generalUseBiomet() == 2);

    biomExtDirEdit->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirBrowse->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirRecCheckBox->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirSuffixLabel->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirCombo->setEnabled(ecProject_->generalUseBiomet() == 3);
}

void ProjectPage::on_biomRadioGroup_clicked_1(int button)
{
    ecProject_->setGeneralUseBiomet(button + 1);
}

void ProjectPage::on_biomRadioGroup_clicked_2(int button)
{
    switch (button)
    {
        case 0:
            biomExtFileEdit->setEnabled(false);
            biomExtFileLoad->setEnabled(false);
            biomExtDirEdit->setEnabled(false);
            biomExtDirBrowse->setEnabled(false);
            biomExtDirRecCheckBox->setEnabled(false);
            biomExtDirSuffixLabel->setEnabled(false);
            biomExtDirCombo->setEnabled(false);
            break;
        case 1:
            biomExtFileEdit->setEnabled(true);
            biomExtFileLoad->setEnabled(true);
            biomExtDirEdit->setEnabled(false);
            biomExtDirBrowse->setEnabled(false);
            biomExtDirRecCheckBox->setEnabled(false);
            biomExtDirSuffixLabel->setEnabled(false);
            biomExtDirCombo->setEnabled(false);
            break;
        case 2:
            biomExtFileEdit->setEnabled(false);
            biomExtFileLoad->setEnabled(false);
            biomExtDirEdit->setEnabled(true);
            biomExtDirBrowse->setEnabled(true);
            biomExtDirRecCheckBox->setEnabled(true);
            biomExtDirSuffixLabel->setEnabled(true);
            biomExtDirCombo->setEnabled(true);
            break;
        default:
            break;
    }
}

void ProjectPage::updateExtDirRec(bool b)
{
    ecProject_->setGeneralBiomRecurse(b);
}

void ProjectPage::on_biomDirBrowse_clicked()
{
    DEBUG_FUNC_NAME

    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString dir = QFileDialog::getExistingDirectory(this,
                    tr("Select the Biomet Files Directory"),
                    searchPath
                    );

    if (!dir.isEmpty())
    {
        QDir dataDir(dir);
        QString canonicalDataDir = dataDir.canonicalPath();
        biomExtDirEdit->setText(QDir::toNativeSeparators(canonicalDataDir));

        configState_->window.last_data_path = canonicalDataDir;
        Alia::updateLastDatapath(canonicalDataDir);
    }
}

void ProjectPage::updateExtDirSuffix(const QString& s)
{
    DEBUG_FUNC_NAME
    if (s.isEmpty())
    {
        QMessageBox::warning(0,
                             tr("Files suffix"),
                             tr("Enter a non empty string."));

        biomExtDirCombo->setCurrentIndex(0);
        return;
    }
    ecProject_->setGeneralBiomExt(s);
}

void ProjectPage::clearDynamicMdEdit()
{
    dynamicMdEdit->clear();
    Alia::updateLineEditToolip(dynamicMdEdit);
}

void ProjectPage::clearBiomExtFileEdit()
{
    biomExtFileEdit->clear();
    Alia::updateLineEditToolip(biomExtFileEdit);
}

void ProjectPage::clearBiomExtDirEdit()
{
    biomExtDirEdit->clear();
    Alia::updateLineEditToolip(biomExtDirEdit);
}

void ProjectPage::updateSmartfluxBar()
{
    DEBUG_FUNC_NAME
    qDebug() << configState_->project.smartfluxMode;
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}