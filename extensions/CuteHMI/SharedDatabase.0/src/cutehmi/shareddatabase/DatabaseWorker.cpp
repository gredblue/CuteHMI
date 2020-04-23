#include <cutehmi/shareddatabase/DatabaseWorker.hpp>

#include "internal/DatabaseDictionary.hpp"

#include <QSqlDatabase>
#include <QSqlDriver>

namespace cutehmi {
namespace shareddatabase {

DatabaseWorker::DatabaseWorker(const QString & connectionName, std::function<void (QSqlDatabase & db)> task):
	m(new Members{connectionName, task, nullptr, {}})
{
	connect(& internal::DatabaseDictionary::Instance(), & internal::DatabaseDictionary::threadChanged, this, & DatabaseWorker::updateDbThread);
	instantiateWorker();
}

void DatabaseWorker::setTask(std::function<void (QSqlDatabase & db)> task)
{
	m->task = task;
}

void DatabaseWorker::job(QSqlDatabase & db)
{
	if (m->task)
		m->task(db);
}

void DatabaseWorker::wait() const
{
	m->worker->wait();
}

bool DatabaseWorker::isReady() const
{
	return m->worker->isReady();
}

bool DatabaseWorker::isWorking() const
{
	return m->worker->isWorking();
}

void DatabaseWorker::work()
{
	emit started();
	m->worker->work();
}


QThread * DatabaseWorker::dbThread() const
{
	return internal::DatabaseDictionary::Instance().associatedThread(m->connectionName);
}

QSqlDatabase * DatabaseWorker::db()
{
	return m->db.get();
}

const QSqlDatabase * DatabaseWorker::db() const
{
	return m->db.get();
}

void DatabaseWorker::updateDbThread(const QString & connectionName)
{
	if (connectionName == m->connectionName) {
		if (m->worker)
			m->worker->wait();
		instantiateWorker();
	}
}

void DatabaseWorker::instantiateWorker()
{
	m->worker.reset(new Worker([this]() {
		m->db.reset(new QSqlDatabase(QSqlDatabase::database(m->connectionName)));
		if (!m->db->isOpen()) {
			CUTEHMI_CRITICAL("Database worker '" << this << "' refuses to do the job, because database connection '" << m->connectionName << "' is not open.");
			emit striked(QObject::tr("database connection '%1' is not open").arg(m->connectionName));
		} else
			job(*m->db);
		m->db.reset();
	}));
	if (dbThread()) {
		if (dbThread() == QThread::currentThread())
			CUTEHMI_DEBUG("Database worker for connection '" << m->connectionName << "' will operate from current thread.");
		else {
			m->worker->employ(*dbThread(), false);
			CUTEHMI_DEBUG("Database worker for connection '" << m->connectionName << "' has been employed in dedicated database thread.");
		}
	} else
		CUTEHMI_WARNING("Database worker for connection '" << m->connectionName << "' will operate from current thread, because upon its creation there was no dedicated database thread associated with that connection in shared database dictionary.");
	connect(m->worker.get(), & Worker::ready, this, & DatabaseWorker::ready);
}

}
}