#include <cutehmi/dataacquisition/EventWriter.hpp>

namespace cutehmi {
namespace dataacquisition {

EventWriter::EventWriter(QObject * parent):
	AbstractWriter(parent),
	m(new Members)
{
	connect(this, & AbstractWriter::schemaChanged, this, & EventWriter::onSchemaChanged);
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureStarting(QState * starting)
{
	return configureStartingOrRepairing(starting);
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureStarted(QState * active, const QState * idling, const QState * yielding)
{
	Q_UNUSED(yielding)
	Q_UNUSED(idling)

	std::unique_ptr<services::Serviceable::ServiceStatuses> statuses = std::make_unique<services::Serviceable::ServiceStatuses>();

	QState * receivingEvents = new QState(active);
	active->setInitialState(receivingEvents);
	statuses->insert(receivingEvents, tr("Listening for events"));

	connect(receivingEvents, & QState::entered, this, & EventWriter::connectTagSignals);
	connect(receivingEvents, & QState::exited, this, & EventWriter::disconnectTagSignals);

	return statuses;
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureStopping(QState * stopping)
{
	std::unique_ptr<services::Serviceable::ServiceStatuses> statuses = std::make_unique<services::Serviceable::ServiceStatuses>();

	QState * waitingForWorkers = new QState(stopping);
	stopping->setInitialState(waitingForWorkers);
	statuses->insert(waitingForWorkers, tr("Waiting for database workers to finish"));
	connect(waitingForWorkers, & QState::entered, & m->dbCollective, & internal::EventCollective::confirmWorkersFinished);

	return statuses;
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureBroken(QState * broken)
{
	Q_UNUSED(broken)

	return nullptr;
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureRepairing(QState * repairing)
{
	return configureStartingOrRepairing(repairing);
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureEvacuating(QState * evacuating)
{
	connect(evacuating, & QState::entered, this, & EventWriter::stopped);

	return nullptr;
}

std::unique_ptr<QAbstractTransition> EventWriter::transitionToStarted() const
{
	connect(this, & EventWriter::schemaValidated, this, & EventWriter::started);

	return std::make_unique<QSignalTransition>(this, & EventWriter::started);
}

std::unique_ptr<QAbstractTransition> EventWriter::transitionToStopped() const
{
	connect(& m->dbCollective, & internal::EventCollective::workersFinished, this, & EventWriter::stopped);

	return std::make_unique<QSignalTransition>(this, & EventWriter::stopped);
}

std::unique_ptr<QAbstractTransition> EventWriter::transitionToBroken() const
{
	connect(& m->dbCollective, & internal::EventCollective::errored, this, & EventWriter::broke);

	return std::make_unique<QSignalTransition>(this, & EventWriter::broke);
}

std::unique_ptr<QAbstractTransition> EventWriter::transitionToYielding() const
{
	return nullptr;
}

std::unique_ptr<QAbstractTransition> EventWriter::transitionToIdling() const
{
	return nullptr;
}

void EventWriter::onSchemaChanged()
{
	m->dbCollective.setSchema(schema());
}

void EventWriter::insertEvent(TagValue * tag)
{
	m->dbCollective.insert(*tag);
}

void EventWriter::connectTagSignals()
{
	for (TagValueContainer::const_iterator it = values().begin(); it != values().end(); ++it) {
		QObject::connect(*it, & TagValue::valueChanged, this, [it, this]() {
			insertEvent(*it);
		});
	}
}

void EventWriter::disconnectTagSignals()
{
	for (TagValueContainer::const_iterator it = values().begin(); it != values().end(); ++it)
		(*it)->disconnect(this);
}

std::unique_ptr<services::Serviceable::ServiceStatuses> EventWriter::configureStartingOrRepairing(QState * parent)
{
	std::unique_ptr<services::Serviceable::ServiceStatuses> statuses = std::make_unique<services::Serviceable::ServiceStatuses>();

	QState * validatingSchema = createValidatingSchemaSate(parent,  statuses.get());

	QState * waitingForDatabase = createWaitingForDatabaseConnectedSate(parent, statuses.get(), validatingSchema);
	parent->setInitialState(waitingForDatabase);

	return statuses;
}

}
}

//(c)C: Copyright © 2020, Michał Policht <michal@policht.pl>. All rights reserved.
//(c)C: This file is a part of CuteHMI.
//(c)C: CuteHMI is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//(c)C: CuteHMI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//(c)C: You should have received a copy of the GNU Lesser General Public License along with CuteHMI.  If not, see <https://www.gnu.org/licenses/>.
