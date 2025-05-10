#ifndef DUOLINGO_H
#define DUOLINGO_H

#include <QAudioOutput>
#include <QButtonGroup>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPair>
#include <QProgressBar>
#include <QRadioButton>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QTimer>
#include <QVector>

class Duolingo : public QMainWindow {
    Q_OBJECT

   public:
    explicit Duolingo(QWidget* parent = nullptr);
    ~Duolingo() override;

   protected:
    void keyPressEvent(QKeyEvent* event) override;

   private slots:
    void OnTranslationClicked();
    void OnGrammarClicked();
    void OnSubmitClicked();
    void OnTimeout();
    void UpdateTimer();
    void ShowHelp();
    void ShowDifficultyDialog();
    void ShowRating();

   private:
    QStackedWidget* stackedWidget;
    QProgressBar* progressBar;
    QLabel* scoreLabel;
    QLabel* timerLabel;
    QTimer* exerciseTimer;
    QTimer* countdownTimer;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;

    int currentExercise;
    int score;
    int wrongAttempts;
    int timeLeft;
    QString currentDifficulty;
    QVector<QPair<QString, QString>> translationExercises;
    QVector<QPair<QString, QString>> grammarExercises;
    QVector<QString> grammarQuestions;
    QVector<QVector<QString>> grammarOptions;
    QVector<int> grammarAnswers;

    void SetupExercises();
    void LearnPage();
    void TranslationPage();
    void GrammarPage();
    void BeginPage() const;
    void ShowTranslationExercise();
    void ShowGrammarExercise();
    void CheckTranslationAnswer();
    void CheckGrammarAnswer();
    void FinishExercise(bool success);
    void PlaySound(bool correct);
    bool IsAnswerCorrect(const QString& userAnswer, const QString& correctAnswer);
};

#endif  // DUOLINGO_H