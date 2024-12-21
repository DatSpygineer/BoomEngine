
extern "C" {
	#include "puli_dialog.h"
	#include "puli_fsys.h"
	#include "puli_str.h"
}

#include <string>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <climits>
	#include <unistd.h>
	#include <QApplication>
	#include <QMessageBox>
	#include <QFileDialog>

static auto GetMessageboxIcon(MessageBoxType type) {
	switch (type) {
		case MSGBOX_INFO: return QMessageBox::Information;
		case MSGBOX_WARNING: return QMessageBox::Warning;
		case MSGBOX_ERROR: return QMessageBox::Critical;
		case MSGBOX_QUESTION: return QMessageBox::Question;
	}
	return QMessageBox::NoIcon;
}
static QFlags<QMessageBox::StandardButton> GetMessageboxButtons(MessageBoxButtons btn) {
	switch (btn) {
		case MSGBOX_OK_CANCEL: return QMessageBox::Ok | QMessageBox::Cancel;
		case MSGBOX_YES_NO: return QMessageBox::Yes | QMessageBox::No;
		case MSGBOX_YES_NO_CANCEL: return QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
		default: case MSGBOX_OK: return QMessageBox::Ok;
	}
}

static auto GetFilterString(const FileSelectFilter* filters, size_t filter_count) {
	QString qfilters = "All files (*.*)";
	for (int i = 0; i < filter_count; i++) {
		qfilters += QString::asprintf("%s (%s)", filters[i].name, filters[i].pattern);
		if (i != filter_count - 1) {
			qfilters += ";;";
		}
	}
	return qfilters;
}

static QApplication* qAppInst;
#endif

void pSetupDialogs(int argc, char** argv) {
#ifdef _WIN32
#else
	qAppInst = new QApplication(argc, argv);
#endif
}
void pDisposeDialogs() {
#ifdef _WIN32
#else
	delete qAppInst;
#endif
}

MessageBoxResult pMessageBox(const char* title, const char* message, MessageBoxType type, MessageBoxButtons buttons) {
#ifdef _WIN32

#else
	QMessageBox msgbox;
	msgbox.setWindowTitle(title);
	msgbox.setText(message);
	msgbox.setIcon(GetMessageboxIcon(type));
	msgbox.setStandardButtons(GetMessageboxButtons(buttons));
	switch (msgbox.exec()) {
		case QMessageBox::Ok: return RESULT_OK;
		case QMessageBox::Cancel: return RESULT_CANCEL;
		case QMessageBox::Yes: return RESULT_YES;
		case QMessageBox::No: return RESULT_NO;
		default: return RESULT_UNKNOWN;
	}
#endif
}

MessageBoxResult pMessageBoxFormat(const char* title, MessageBoxType type, MessageBoxButtons buttons, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	MessageBoxResult result = pMessageBoxFormatV(title, type, buttons, fmt, args);
	va_end(args);
	return result;
}

MessageBoxResult pMessageBoxFormatV(const char* title, MessageBoxType type, MessageBoxButtons buttons, const char* fmt, va_list args) {
	char temp[2048] = { 0 };
	vsnprintf(temp, 2048, fmt, args);
	return pMessageBox(title, temp, type, buttons);
}

size_t pOpenFileDialogMultiple(char** result, const char* title, int max_count, const FileSelectFilter* filters, size_t filter_count) {
#ifdef _WIN32
#else
	auto qfilters = GetFilterString(filters, filter_count);

	if (max_count <= 1) {
		auto qresult = QFileDialog::getOpenFileName(nullptr, title, QString(), qfilters);
		if (qresult.isEmpty()) {
			return 0;
		}
		pStrCpy(*result, P_PATH_MAX, qresult.toUtf8());
		return 1;
	}

	auto qresults = QFileDialog::getOpenFileNames(nullptr, title, QString(), qfilters);
	if (!qresults.isEmpty()) {
		int i = 0;
		for (auto file : qresults) {
			if (i >= max_count) {
				break;
			}
			pStrCpy(result[i++], P_PATH_MAX, file.toUtf8());
		}
		return i;
	}
	return 0;
#endif
}
size_t pSaveFileDialog(char* result, const char* title, const FileSelectFilter* filters, size_t filter_count) {
#ifdef _WIN32
#else
	auto qresult = QFileDialog::getSaveFileName(nullptr, title, QString(), GetFilterString(filters, filter_count));
	if (qresult.isEmpty()) {
		return 0;
	}
	pStrCpy(result, P_PATH_MAX, qresult.toUtf8());
	return qresult.size();
#endif
}
size_t pOpenDirDialog(char* result, const char* title) {
#ifdef _WIN32
#else
	auto qresult = QFileDialog::getExistingDirectory(nullptr, title);
	if (qresult.isEmpty()) {
		return 0;
	}
	pStrCpy(result, P_PATH_MAX, qresult.toUtf8());
	return qresult.size();
#endif
}