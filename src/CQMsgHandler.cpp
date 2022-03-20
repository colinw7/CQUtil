#include <CQMsgHandler.h>

#include <QApplication>

namespace CQMsgHandler {

bool
checkEnv(const char *name) {
  const char *p = getenv(name);
  if (! p) return false;

  QString s = p;

  return (s == "1" || s == "true" || s == "yes");
}

bool
isIgnoreMsg(const QString &msg)
{
  static QStringList ignoreMessages;

  if (ignoreMessages.empty()) {
    ignoreMessages.push_back("QBasicTimer can only be used with threads started with QThread");
  }

  for (const auto &imsg : ignoreMessages) {
    if (msg.indexOf(imsg) > 0)
      return true;
  }

  return false;
}

void
messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  auto localMsg = msg.toLocal8Bit();

  if (isIgnoreMsg(localMsg))
    return;

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
  qInstallMessageHandler(messageOutput);
}

}
