#include "qlabel.h"
#include <ostream>
#include <sstream>  // For std::ostringstream

class QLabelStreamBuffer : public std::streambuf {
public:
    QLabelStreamBuffer(std::ostream &stream, QLabel *label) : m_stream(stream), m_label(label) {
        m_oldBuf = stream.rdbuf(this);  // Redirect stream buffer to this one
    }

    ~QLabelStreamBuffer() {
        m_stream.rdbuf(m_oldBuf);  // Restore the old buffer
    }

protected:
    virtual int overflow(int c = EOF) override {
        if (c != EOF) {
            m_buffer += static_cast<char>(c);
        }
        if (c == '\n' || c == '\r') {
            flushBuffer();  // Flush buffer when a newline is encountered
        }
        return c;
    }

    virtual int sync() override {
        flushBuffer();  // Ensure any remaining buffer is flushed
        return 0;
    }

private:
    void flushBuffer() {
        if (!m_buffer.empty()) {
            // Append text to QLabel safely using invokeMethod for thread-safety
            QMetaObject::invokeMethod(m_label, "setText", Qt::QueuedConnection,
                                      Q_ARG(QString, m_label->text() + QString::fromStdString(m_buffer)));
            m_buffer.clear();  // Clear the buffer after flushing
        }
    }

    std::ostream &m_stream;
    std::streambuf *m_oldBuf;
    std::string m_buffer;
    QLabel *m_label;
};
