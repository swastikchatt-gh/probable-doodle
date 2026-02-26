#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QProcess>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QIcon>
#include <QStatusBar>
#include <QLabel>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QFontDatabase>   // for system font

class TerminalWindow : public QMainWindow
{
    Q_OBJECT

public:
    TerminalWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("MyTerminal");
        setWindowIcon(QIcon(":/app-icon")); // embedded icon

        // Central widget with layout
        QWidget *central = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(central);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        // Output area
        output = new QTextEdit(this);
        output->setReadOnly(true);
        output->setLineWrapMode(QTextEdit::NoWrap);
        layout->addWidget(output);

        // Input line
        input = new QLineEdit(this);
        layout->addWidget(input);

        // Set monospace font using system default fixed font
        QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        output->setFont(fixedFont);
        input->setFont(fixedFont);

        setCentralWidget(central);

        // Create actions and menus
        createActions();
        createMenus();

        // Status bar
        statusLabel = new QLabel(tr("Not connected"));
        statusBar()->addWidget(statusLabel);

        // Start the shell
        startShell();

        // Connect input return pressed to send command
        connect(input, &QLineEdit::returnPressed, this, &TerminalWindow::sendCommand);

        // Set focus to input
        input->setFocus();
    }

    ~TerminalWindow()
    {
        if (shell && shell->state() != QProcess::NotRunning) {
            shell->terminate();
            shell->waitForFinished(1000);
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (shell && shell->state() != QProcess::NotRunning) {
            shell->terminate();
            if (!shell->waitForFinished(1000)) {
                shell->kill();
            }
        }
        event->accept();
    }

private slots:
    void sendCommand()
    {
        if (!shell || shell->state() != QProcess::Running) {
            QMessageBox::warning(this, tr("Terminal"),
                                 tr("Shell is not running."));
            return;
        }

        QString cmd = input->text();
        if (!cmd.isEmpty()) {
            // Echo the command in the output (optional, shell usually does it)
            // output->append("$ " + cmd);
            shell->write((cmd + "\n").toLocal8Bit());
        }
        input->clear();
    }

    void readOutput()
    {
        if (shell) {
            QByteArray data = shell->readAllStandardOutput();
            output->moveCursor(QTextCursor::End);
            output->insertPlainText(QString::fromLocal8Bit(data));
        }
    }

    void readError()
    {
        if (shell) {
            QByteArray data = shell->readAllStandardError();
            output->moveCursor(QTextCursor::End);
            output->insertPlainText(QString::fromLocal8Bit(data));
        }
    }

    void shellStarted()
    {
        statusLabel->setText(tr("Shell running"));
    }

    void shellFinished(int exitCode, QProcess::ExitStatus status)
    {
        QString msg = tr("Shell finished (exit code %1)").arg(exitCode);
        if (status == QProcess::CrashExit)
            msg = tr("Shell crashed");
        statusLabel->setText(msg);
        input->setEnabled(false);
        output->append(msg);
    }

    void shellError(QProcess::ProcessError error)
    {
        QString msg;
        switch (error) {
        case QProcess::FailedToStart:
            msg = tr("Shell failed to start");
            break;
        case QProcess::Crashed:
            msg = tr("Shell crashed");
            break;
        default:
            msg = tr("Shell error");
        }
        statusLabel->setText(msg);
        output->append(msg);
        input->setEnabled(false);
    }

    void about()
    {
        QMessageBox::about(this, tr("About MyTerminal"),
            tr("MyTerminal – A simple Qt6 terminal emulator.\n\n"
               "Uses your default shell and provides a basic command interface.\n"
               "Icon from icons/image.png."));
    }

private:
    void createActions()
    {
        // No file actions besides quit for this simple terminal
        exitAct = new QAction(tr("&Quit"), this);
        exitAct->setShortcut(QKeySequence::Quit);
        connect(exitAct, &QAction::triggered, this, &QWidget::close);

        copyAct = new QAction(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
        copyAct->setShortcut(QKeySequence::Copy);
        connect(copyAct, &QAction::triggered, output, &QTextEdit::copy);

        pasteAct = new QAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
        pasteAct->setShortcut(QKeySequence::Paste);
        connect(pasteAct, &QAction::triggered, input, &QLineEdit::paste);

        aboutAct = new QAction(tr("&About"), this);
        connect(aboutAct, &QAction::triggered, this, &TerminalWindow::about);
    }

    void createMenus()
    {
        QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(exitAct);

        QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
        editMenu->addAction(copyAct);
        editMenu->addAction(pasteAct);

        QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
        helpMenu->addAction(aboutAct);
    }

    void startShell()
    {
        QString shellPath = QString::fromLocal8Bit(qgetenv("SHELL"));
        if (shellPath.isEmpty())
            shellPath = "/bin/sh";

        shell = new QProcess(this);
        connect(shell, &QProcess::readyReadStandardOutput, this, &TerminalWindow::readOutput);
        connect(shell, &QProcess::readyReadStandardError, this, &TerminalWindow::readError);
        connect(shell, &QProcess::started, this, &TerminalWindow::shellStarted);
        connect(shell, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &TerminalWindow::shellFinished);
        connect(shell, &QProcess::errorOccurred, this, &TerminalWindow::shellError);

        // Set environment to make the shell think it's interactive
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("TERM", "xterm-256color");
        env.insert("PS1", "\\u@\\h:\\w $ "); // optional custom prompt
        shell->setProcessEnvironment(env);

        // Start the shell with appropriate arguments for interactive use
        shell->setProgram(shellPath);
        shell->setArguments({"-i"});

        output->append(tr("Starting %1...\n").arg(shellPath));
        shell->start();

        if (!shell->waitForStarted(3000)) {
            output->append(tr("Failed to start shell."));
            input->setEnabled(false);
        }
    }

    QTextEdit *output;
    QLineEdit *input;
    QLabel *statusLabel;
    QProcess *shell;

    QAction *exitAct, *copyAct, *pasteAct, *aboutAct;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TerminalWindow w;
    w.show();
    return app.exec();
}

#include "main.moc"
