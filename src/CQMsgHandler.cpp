#include <CQMsgHandler.h>

#include <QApplication>

namespace {

bool checkEnv(const char *name) {
  const char *p = getenv(name);
  if (! p) return false;

  QString s = p;

  return (s == "1" || s == "true" || s == "yes");
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QByteArray localMsg = msg.toLocal8Bit();

  switch (type) {
    case QtDebugMsg: {
      fprintf(stderr, "Debug: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      break;
    }
    case QtInfoMsg: {
      fprintf(stderr, "Info: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      break;
    }
    case QtWarningMsg: {
      fprintf(stderr, "Warning: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);

      if (checkEnv("CQMSG_HANDLER_WARN_ASSERT"))
        abort();

      break;
    }
    case QtCriticalMsg: {
      fprintf(stderr, "Critical: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      abort();
      break;
    }
    case QtFatalMsg: {
      fprintf(stderr, "Fatal: %s (%s:%u, %s)\n",
              localMsg.constData(), context.file, context.line, context.function);
      abort();
      break;
    }
  }
}

}

namespace CQMsgHandler {

void
install()
{
  qInstallMessageHandler(myMessageOutput);
}

}
