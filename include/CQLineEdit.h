#ifndef CQLineEdit_H
#define CQLineEdit_H

#include <QLineEdit>

/*!
 * Simple wrapper around QLineEdit to support colored placeholder text
*/
class CQLineEdit : public QLineEdit {
  Q_OBJECT

  Q_PROPERTY(QColor placeholderColor  READ placeholderColor  WRITE setPlaceholderColor )
  Q_PROPERTY(bool   placeholderItalic READ placeholderItalic WRITE setPlaceholderItalic)

 private:
  enum State {
    STATE_NONE,
    STATE_NORMAL,
    STATE_PLACE
  };

 public:
  //! create widget
  CQLineEdit(QWidget *parent=0);

  //! get placeholder text color
  const QColor &placeholderColor() const { return placeColor_; }
  //! set placeholder text color
  void setPlaceholderColor(const QColor &color);

  //! get placeholder is italic
  bool placeholderItalic() const { return placeItalic_; }
  //! set placeholder is italic
  void setPlaceholderItalic(bool italic);

 private slots:
  //! update placeholder style state
  void updateState(bool force=false);

 private:
  //! update style sheet
  void updateStyle();

  //! get stylesheet color string from color
  static QString colorString(const QColor &color);

 protected:
  //! handle focus in event
  void focusInEvent(QFocusEvent *e) override;

  //! handle focus out event
  void focusOutEvent(QFocusEvent *e) override;

  //! handle change event
  void changeEvent(QEvent *e) override;

 private:
  QColor placeColor_;  //!< placeholder text color
  bool   placeItalic_; //!< placeholder is italic
  State  state_;       //!< current placeholder style state
  bool   ignore_;      //!< ignore events
};

#endif
