#ifndef DATABASECOLLECTIVE_HPP
#define DATABASECOLLECTIVE_HPP

#include "common.hpp"
#include "../Schema.hpp"

#include <QObject>

namespace cutehmi {
namespace dataacquisition {
namespace internal {

class CUTEHMI_DATAACQUISITION_PRIVATE TableCollective:
	public QObject
{
		Q_OBJECT

	public:
		TableCollective();

		Schema * schema() const;

		void setSchema(Schema * schema);

	public slots:
		void confirmWorkersFinished();

	signals:
		void workersFinished();

		void errored(cutehmi::InplaceError error);

	protected:
		virtual void updateSchema(Schema * schema) = 0;

		void accountInsertBusy(bool busy);

	private:
		struct Members
		{
			Schema * schema = nullptr;
			int insertsBusy = 0;
		};

		MPtr<Members> m;
};

}
}
}

#endif // DATABASECOLLECTIVE_HPP