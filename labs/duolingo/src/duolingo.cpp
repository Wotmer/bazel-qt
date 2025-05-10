#include "../include/duolingo.h"

#include <QAudioOutput>
#include <QComboBox>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QTextEdit>
#include <QVBoxLayout>

// TODO добавить возможность переводить предложения, где надо вставить текст

Duolingo::Duolingo(QWidget* parent)
    : QMainWindow(parent)
    , player(new QMediaPlayer(this))
    , currentExercise(0)
    , score(0)
    , wrongAttempts(0)
    , currentDifficulty("start") {
    // панель сверху
    QMenu* menu = menuBar()->addMenu("Меню");
    QAction* difficulty = menu->addAction("Выбрать сложность");
    QAction* rating = menu->addAction("Рейтинг пользователей");
    QAction* help = menu->addAction("Помощь");
    connect(difficulty, &QAction::triggered, this, &Duolingo::ShowDifficultyDialog);
    connect(help, &QAction::triggered, this, &Duolingo::ShowHelp);
    connect(rating, &QAction::triggered, this, &Duolingo::ShowRating);

    QWidget* centralWidget = new QWidget(this);            // NOLINT
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);  // NOLINT

    stackedWidget = new QStackedWidget;                    // NOLINT
    BeginPage();
    LearnPage();
    TranslationPage();
    GrammarPage();
    QComboBox* pageComboBox = new QComboBox;  // NOLINT
    pageComboBox->addItem(tr("Главная"));
    pageComboBox->addItem(tr("Cлова"));
    pageComboBox->addItem(tr("Перевод"));
    pageComboBox->addItem(tr("Грамматика"));
    connect(
        pageComboBox, QOverload<int>::of(&QComboBox::activated), stackedWidget,
        &QStackedWidget::setCurrentIndex);

    layout->addWidget(pageComboBox, 5, Qt::AlignRight);
    layout->addWidget(stackedWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    /*QWidget* centralWidget = new QWidget(this);                // NOLINT
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);  // NOLINT

    // выбор упражнения сверху
    QGroupBox* menuGroup = new QGroupBox();                 // NOLINT
    QVBoxLayout* menuLayout = new QVBoxLayout();            // NOLINT
    QPushButton* learn = new QPushButton("Cлова");          // NOLINT
    QPushButton* translation = new QPushButton("Перевод");  // NOLINT
    QPushButton* grammar = new QPushButton("Грамматика");   // NOLINT
    menuGroup->setFixedWidth(120);
    menuLayout->addWidget(learn);
    menuLayout->addWidget(translation);
    menuLayout->addWidget(grammar);
    menuGroup->setLayout(menuLayout);///////////////////

    QWidget* statusWidget = new QWidget();          // NOLINT
    QHBoxLayout* statusLayout = new QHBoxLayout();  // NOLINT

    progressBar = new QProgressBar();               // NOLINT
    progressBar->setRange(0, 5);
    progressBar->setValue(0);

    scoreLabel = new QLabel("Результат: 0");  // NOLINT
    timerLabel = new QLabel("Время: 00:00");  // NOLINT

    statusLayout->addWidget(progressBar);
    statusLayout->addWidget(scoreLabel);
    statusLayout->addWidget(timerLabel);
    statusWidget->setLayout(statusLayout);

    QPushButton* submit = new QPushButton("Отправить");  // NOLINT
    QVBoxLayout* rightLayout = new QVBoxLayout();        // NOLINT
    rightLayout->addWidget(stackedWidget);
    rightLayout->addWidget(statusWidget);
    rightLayout->addWidget(submit);

    mainLayout->addWidget(menuGroup, 1);
    mainLayout->addLayout(rightLayout, 3);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(translation, &QPushButton::clicked, this, &Duolingo::OnTranslationClicked);
    connect(grammar, &QPushButton::clicked, this, &Duolingo::OnGrammarClicked);
    connect(submit, &QPushButton::clicked, this, &Duolingo::OnSubmitClicked);

    exerciseTimer = new QTimer(this);
    countdownTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &Duolingo::OnTimeout);
    connect(countdownTimer, &QTimer::timeout, this, &Duolingo::UpdateTimer);

    SetupExercises();*/
}

void Duolingo::BeginPage() const {
    QVBoxLayout* welcomeLayout = new QVBoxLayout();                                       // NOLINT
    QWidget* welcomeScreen = new QWidget();                                               // NOLINT
    welcomeScreen->setLayout(welcomeLayout);
    QLabel* text = new QLabel("Добро пожаловать в приложение для изучения испанского!");  // NOLINT
    if (currentDifficulty == "start") {
        qDebug() << currentDifficulty;
        text = new QLabel( // NOLINT
            "Добро пожаловать в приложение для изучения испанского!\n"                    // NOLINT
            "Пройдите тест, чтобы определить Ваш уровень владения языком\n(позже этот уровень "  // NOLINT
            "можно будет изменить в меню)");  // NOLINT
    }
    welcomeLayout->addWidget(text, 0, Qt::AlignCenter);
    stackedWidget->addWidget(welcomeScreen);
}

void Duolingo::LearnPage() {
    QWidget* learnPageWidget = new QWidget;            // NOLINT
    QHBoxLayout* learnPageLayout = new QHBoxLayout();  // NOLINT
    QHBoxLayout* statusLayout = new QHBoxLayout();     // NOLINT

    progressBar = new QProgressBar();                  // NOLINT
    progressBar->setRange(0, 5);
    progressBar->setValue(0);

    scoreLabel = new QLabel("Результат: 0");             // NOLINT
    timerLabel = new QLabel("Время: 00:00");             // NOLINT

    QPushButton* submit = new QPushButton("Отправить");  // NOLINT

    statusLayout->addWidget(progressBar);
    statusLayout->addWidget(scoreLabel);
    statusLayout->addWidget(timerLabel);
    learnPageLayout->addLayout(statusLayout);
    learnPageLayout->addWidget(submit);

    exerciseTimer = new QTimer(this);
    countdownTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &Duolingo::OnTimeout);
    connect(countdownTimer, &QTimer::timeout, this, &Duolingo::UpdateTimer);

    SetupExercises();

    stackedWidget->addWidget(learnPageWidget);
}

void Duolingo::TranslationPage() {
    QWidget* translationPageWidget = new QWidget;  // NOLINT
    stackedWidget->addWidget(translationPageWidget);
}

void Duolingo::GrammarPage() {
    QWidget* grammarPageWidget = new QWidget;  // NOLINT
    stackedWidget->addWidget(grammarPageWidget);
}

Duolingo::~Duolingo() {
    delete player;
    delete exerciseTimer;
    delete countdownTimer;
}

void Duolingo::SetupExercises() {
    translationExercises = {
      {"Привет", "Hola"}, {"Пока", "Adios"}, {"Спасибо", "Gracias"}, {"Пожалуйста", ""}};

    grammarQuestions = {
      "I ___ (to eat) an apple", "She ___ (to go) to school", "We ___ (to have) a cat",
      "They ___ (to be) happy", "You ___ (to speak) French"};

    grammarOptions = {
      {"eat", "eats", "eating", "ate"},
      {"go", "goes", "going", "went"},
      {"have", "has", "having", "had"},
      {"are", "is", "were", "be"},
      {"speak", "speaks", "speaking", "spoke"}};

    grammarAnswers = {0, 1, 0, 0, 0};
}

void Duolingo::OnTranslationClicked() {
    currentExercise = 0;
    wrongAttempts = 0;
    progressBar->setMaximum(translationExercises.size());  // NOLINT
    progressBar->setValue(0);

    if (currentDifficulty == "Начинающий") {
        timeLeft = 120;
    } else if (currentDifficulty == "Продвинутый") {
        timeLeft = 90;
    } else {
        timeLeft = 60;
    }

    timerLabel->setText(QString("Time: %1:%2")
                            .arg(timeLeft / 60, 2, 10, QLatin1Char('0'))
                            .arg(timeLeft % 60, 2, 10, QLatin1Char('0')));

    countdownTimer->start(1000);
    exerciseTimer->start(timeLeft * 1000);

    ShowTranslationExercise();
}

void Duolingo::OnGrammarClicked() {
    currentExercise = 0;
    wrongAttempts = 0;
    progressBar->setMaximum(grammarQuestions.size());  // NOLINT
    progressBar->setValue(0);

    if (currentDifficulty == "Начинающий") {
        timeLeft = 150;
    } else if (currentDifficulty == "Продвинутый") {
        timeLeft = 120;
    } else {
        timeLeft = 90;
    }

    timerLabel->setText(QString("Time: %1:%2")
                            .arg(timeLeft / 60, 2, 10, QLatin1Char('0'))
                            .arg(timeLeft % 60, 2, 10, QLatin1Char('0')));

    countdownTimer->start(1000);
    exerciseTimer->start(timeLeft * 1000);

    ShowGrammarExercise();
}

void Duolingo::ShowTranslationExercise() {
    if (currentExercise >= translationExercises.size()) {
        FinishExercise(true);
        return;
    }

    QWidget* translationWidget = new QWidget();                                   // NOLINT
    QVBoxLayout* layout = new QVBoxLayout();                                      // NOLINT

    QLabel* instruction = new QLabel("Переведите на испанский:");                 // NOLINT
    QLabel* wordLabel = new QLabel(translationExercises[currentExercise].first);  // NOLINT
    QTextEdit* answerEdit = new QTextEdit();                                      // NOLINT

    layout->addWidget(instruction);
    layout->addWidget(wordLabel);
    layout->addWidget(answerEdit);
    translationWidget->setLayout(layout);

    if (stackedWidget->count() > 1) {
        QWidget* oldWidget = stackedWidget->widget(1);
        stackedWidget->removeWidget(oldWidget);
        delete oldWidget;
    }

    stackedWidget->addWidget(translationWidget);
    stackedWidget->setCurrentIndex(1);
}

void Duolingo::ShowGrammarExercise() {
    if (currentExercise >= grammarQuestions.size()) {
        FinishExercise(true);
        return;
    }

    QWidget* grammarWidget = new QWidget();                                           // NOLINT
    QVBoxLayout* layout = new QVBoxLayout();                                          // NOLINT

    QLabel* questionLabel = new QLabel(grammarQuestions[currentExercise]);            // NOLINT
    QButtonGroup* optionsGroup = new QButtonGroup(this);                              // NOLINT

    for (int i = 0; i < grammarOptions[currentExercise].size(); ++i) {
        QRadioButton* option = new QRadioButton(grammarOptions[currentExercise][i]);  // NOLINT
        optionsGroup->addButton(option, i);
        layout->addWidget(option);
    }

    layout->addWidget(questionLabel);
    grammarWidget->setLayout(layout);

    // Replace current widget in stack
    if (stackedWidget->count() > 1) {
        QWidget* oldWidget = stackedWidget->widget(1);
        stackedWidget->removeWidget(oldWidget);
        delete oldWidget;
    }

    stackedWidget->addWidget(grammarWidget);
    stackedWidget->setCurrentIndex(1);
}

void Duolingo::OnSubmitClicked() {
    if (stackedWidget->currentIndex() == 0) {
        return;
    }

    if (progressBar->value() == 0) {
        if (currentExercise < translationExercises.size()) {
            CheckTranslationAnswer();
        } else {
            CheckGrammarAnswer();
        }
    } else {
        QWidget* current = stackedWidget->currentWidget();
        if (current->findChild<QTextEdit*>()) {
            CheckTranslationAnswer();
        } else if (current->findChild<QRadioButton*>()) {
            CheckGrammarAnswer();
        }
    }
}

void Duolingo::CheckTranslationAnswer() {
    QTextEdit* answerEdit = stackedWidget->currentWidget()->findChild<QTextEdit*>();
    if (!answerEdit) {
        return;
    }

    QString userAnswer = answerEdit->toPlainText().trimmed();
    QString correctAnswer = translationExercises[currentExercise].second;

    if (IsAnswerCorrect(userAnswer, correctAnswer)) {
        PlaySound(true);
        currentExercise++;
        progressBar->setValue(progressBar->value() + 1);
        wrongAttempts = 0;

        if (currentExercise < translationExercises.size()) {
            ShowTranslationExercise();
        } else {
            FinishExercise(true);
        }
    } else {
        PlaySound(false);
        wrongAttempts++;
        if (wrongAttempts >= 3) {
            FinishExercise(false);
        } else {
            QMessageBox::warning(this, "Incorrect", "Try again!");
        }
    }
}

void Duolingo::CheckGrammarAnswer() {
    QButtonGroup* optionsGroup = stackedWidget->currentWidget()->findChild<QButtonGroup*>();
    if (!optionsGroup) {
        return;
    }

    int selectedId = optionsGroup->checkedId();
    if (selectedId == -1) {
        QMessageBox::warning(this, "Ничего не выбрано.", "Пожалуйста, выберите ответ!");
        return;
    }

    if (selectedId == grammarAnswers[currentExercise]) {
        PlaySound(true);
        currentExercise++;
        progressBar->setValue(progressBar->value() + 1);
        wrongAttempts = 0;

        if (currentExercise < grammarQuestions.size()) {
            ShowGrammarExercise();
        } else {
            FinishExercise(true);
        }
    } else {
        PlaySound(false);
        wrongAttempts++;
        if (wrongAttempts >= 3) {
            FinishExercise(false);
        } else {
            QMessageBox::warning(this, "Неправильно", "Попробуйте снова!");
        }
    }
}

bool Duolingo::IsAnswerCorrect(const QString& userAnswer, const QString& correctAnswer) {
    // Simple comparison with case insensitivity and ignoring some special characters
    QString simplifiedUser = userAnswer.toLower().trimmed();
    QString simplifiedCorrect = correctAnswer.toLower().trimmed();

    // Remove common punctuation
    simplifiedUser.remove(QRegularExpression("[.,;!?]"));
    simplifiedCorrect.remove(QRegularExpression("[.,;!?]"));

    // Handle common French accents and special characters
    simplifiedUser.replace("é", "e")
        .replace("è", "e")
        .replace("ê", "e")
        .replace("à", "a")
        .replace("â", "a")
        .replace("î", "i")
        .replace("ô", "o")
        .replace("û", "u")
        .replace("ç", "c");

    simplifiedCorrect.replace("é", "e")
        .replace("è", "e")
        .replace("ê", "e")
        .replace("à", "a")
        .replace("â", "a")
        .replace("î", "i")
        .replace("ô", "o")
        .replace("û", "u")
        .replace("ç", "c");

    // Allow for contractions (like "t'aime" vs "te aime")
    simplifiedUser.replace("' ", "'").replace(" '", "'");
    simplifiedCorrect.replace("' ", "'").replace(" '", "'");

    return simplifiedUser == simplifiedCorrect;
}

void Duolingo::FinishExercise(bool success) {
    exerciseTimer->stop();
    countdownTimer->stop();

    if (success) {
        int pointsEarned = 10 * progressBar->maximum();
        score += pointsEarned;
        scoreLabel->setText(QString("Score: %1").arg(score));

        PlaySound(true);
        QMessageBox::information(
            this, "Поздравляем",
            QString("You completed the exercise!\nPoints earned: %1").arg(pointsEarned));
    } else {
        PlaySound(false);
        QMessageBox::information(this, "Exercise Over", "You made too many mistakes. Try again!");
    }

    // Reset to welcome screen
    stackedWidget->setCurrentIndex(0);
    progressBar->setValue(0);
}

void Duolingo::OnTimeout() {
    FinishExercise(false);
    QMessageBox::information(this, "Time's Up", "The time for this exercise has expired.");
}

void Duolingo::UpdateTimer() {
    timeLeft--;
    if (timeLeft <= 0) {
        countdownTimer->stop();
        OnTimeout();
        return;
    }

    timerLabel->setText(QString("Time: %1:%2")
                            .arg(timeLeft / 60, 2, 10, QLatin1Char('0'))
                            .arg(timeLeft % 60, 2, 10, QLatin1Char('0')));
}

void Duolingo::ShowHelp() {
    QString helpText;

    if (stackedWidget->currentIndex() == 0) {
        helpText =
            "Добро пожаловать в приложение для изучения испанского языка!\n\n"
            "Выберите упражнение, чтобы начать:\n"
            "- Новые слова: Появляется новые слова появляются по 5 штук, для перехода к следующим "
            "нужно нажать 'Отправить'\n"
            "- Перевод: Переведите русские слова на испанский\n"
            "- Грамматика: Выберите правильное слово или форму слова\n\n"
            "Также вы можете изменить сложность в меню.";
    } else if (stackedWidget->currentWidget()->findChild<QTextEdit*>()) {
        helpText =
            "Помощь в упражнении на перевод:\n\n"
            "Напишите испанский перевод слова, которое дано вам на русском.\n"
            "Ударения над гласными и уникальные буквы для испанского важны, приложение вам их не "
            "простит.\n"
            "Пример: 'Дерево' должно переводиться как 'Árbol'";
    } else if (stackedWidget->currentWidget()->findChild<QRadioButton*>()) {
        helpText =
            "Помощь в упражнении на грамматику:\n\n"
            "Выберите правильное слово или форму слова, чтобы дополнить предложение.\n"
            "Обращайте внимание на лицо и время.\n"
            "Пример: 'Las ventanas ___ (soy) grandes' должно быть дополнено с помощь 'son'";
    }

    QMessageBox::information(this, "Help", helpText);
}

void Duolingo::ShowDifficultyDialog() {
    QStringList difficulties = {"Beginner", "Intermediate", "Advanced"};
    bool ok;
    QString newDifficulty = QInputDialog::getItem(
        this, "Set Difficulty", "Select difficulty level:", difficulties,
        difficulties.indexOf(currentDifficulty), false, &ok);
    if (ok) {
        currentDifficulty = newDifficulty;
        QMessageBox::information(
            this, "Difficulty Changed", QString("Difficulty set to %1").arg(currentDifficulty));
    }
}

void Duolingo::PlaySound(bool correct) {
    QAudioOutput* audioOutput = new QAudioOutput(this);  // NOLINT
    player->setAudioOutput(audioOutput);

    if (correct) {
        player->setSource(QUrl("qrc:/sounds/correct.wav"));
    } else {
        player->setSource(QUrl("qrc:/sounds/wrong.wav"));
    }
    player->play();
}

void Duolingo::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_H) {
        ShowHelp();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void Duolingo::ShowRating() {
}