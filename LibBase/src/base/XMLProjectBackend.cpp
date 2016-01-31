#include "XMLProjectBackend.hpp"
#include "Error.hpp"
#include "../plugin/IXMLBackend.hpp"

#include <QtDebug>
#include <QLibraryInfo>

namespace base {

QString XMLProjectBackend::Error::str() const
{
	switch (code) {
		case NOT_CUTEHMI_PROJECT_FILE:
			return tr("Not a CuteHMI project file.");
		case UNSUPPORTED_VERSION:
			return tr("Unsupported version of CuteHMI project file.");
		case INVALID_FORMAT:
			return tr("Not a valid CuteHMI project file.");
		case PLUGIN_NOT_LOADED:
			return tr("Could not load plugin.");
		case PLUGIN_WRONG_INTERFACE:
			return tr("Plugin does not implement required interface.");
		case PLUGIN_PARSE_PROBLEM:
			return tr("Plugin was not able to read data properly.");
		default:
			return base::Error::str();
	}
}

XMLProjectBackend::XMLProjectBackend(ProjectModel * model, PluginLoader * pluginLoader):
	m_model(model),
	m_pluginLoader(pluginLoader)
{
}

XMLProjectBackend::Error XMLProjectBackend::load(QIODevice & device)
{
	m_xmlReader.setDevice(& device);

	if (m_xmlReader.readNextStartElement()) {
		if ((m_xmlReader.name() != "cutehmi_project") || (m_xmlReader.namespaceUri() != "http://cutehmi"))
			return Error::NOT_CUTEHMI_PROJECT_FILE;

		QVector<QStringRef> versionMM = m_xmlReader.attributes().value("version").split(".");
		bool ok = true;
		int versionMajor = versionMM.value(0).toInt(& ok);
		if (!ok)
			return Error::INVALID_FORMAT;
		int versionMinor = versionMM.value(1).toInt(& ok);
		if (!ok)
			return Error::INVALID_FORMAT;
		qDebug("Document version is %d.%d", versionMajor, versionMinor); // Note that "qDebug << ver << "." << ver" would output sth like "0 . 0" with emo spaces :/.
		if (versionMajor == 0) {
			Loader0 loader(& m_xmlReader, & m_model->root(), m_pluginLoader);
			return loader.parse(versionMinor);
		} else
			return Error::UNSUPPORTED_VERSION;
	}
	return Error::NOT_CUTEHMI_PROJECT_FILE;
}

//bool XMLProjectBackend::save(QIODevice * device)
//{
//}


XMLProjectBackend::Loader0::Loader0(QXmlStreamReader * xmlReader, ProjectModel::Node * root, PluginLoader * pluginLoader):
	m_xml(xmlReader),
	m_root(root),
	m_pluginLoader(pluginLoader)
{
}

XMLProjectBackend::Error XMLProjectBackend::Loader0::parse(int versionMinor)
{
	qDebug() << "Loader starts parsing document...";
	if (versionMinor > 0)
		return Error::UNSUPPORTED_VERSION;

	Error result = Error::OK;
	while (m_xml->readNextStartElement()) {
		if (m_xml->namespaceUri() != "http://cutehmi") {
			m_xml->skipCurrentElement();
			continue;
		}
		if (m_xml->name() == "plc_clients")
			result = plcClients();
		else if (m_xml->name() == "screens")
			result = screens();
		else
			m_xml->skipCurrentElement();
		if (!result.success())
			return result;
	}
	if (m_xml->hasError())
		return Error::INVALID_FORMAT;
	if (!m_xml->readNextStartElement() && m_xml->atEnd())
		return Error::OK;
	return Error::INVALID_FORMAT;
}

XMLProjectBackend::Error XMLProjectBackend::Loader0::plcClients()
{
	qDebug() << "Entered <plc_clients> section";

	ProjectModel::Node * plcClientsNode = m_root->createChild(ProjectModel::Node::Data("PLC Clients"), false);
	while (m_xml->readNextStartElement()) {
		if (m_xml->namespaceUri() != "http://cutehmi") {
			m_xml->skipCurrentElement();
			continue;
		}
		if (m_xml->name() == "plc_client_plugin") {
			QString pluginName(m_xml->attributes().value("name").toString());
			if (QLibraryInfo::isDebugBuild())
				pluginName.append('d');
			if (!m_pluginLoader->loadPlugin(pluginName, m_xml->attributes().value("version").toString()).success())
				return Error::PLUGIN_NOT_LOADED;
			plugin::IXMLBackend * plugin = qobject_cast<plugin::IXMLBackend *>(m_pluginLoader->instance(pluginName));
			if (plugin == 0)
				return Error::PLUGIN_WRONG_INTERFACE;
			if (!plugin->readXML(*m_xml, *plcClientsNode).success())
				return Error::PLUGIN_PARSE_PROBLEM;
		} else
			m_xml->skipCurrentElement();
	}
	if (m_xml->hasError())
		return Error::INVALID_FORMAT;
	return Error::OK;
}

XMLProjectBackend::Error XMLProjectBackend::Loader0::screens()
{
	qDebug() << "Entered <screens> section";

	m_root->createChild(ProjectModel::Node::Data("Screens"), false);
	while (m_xml->readNextStartElement()) {
		m_xml->skipCurrentElement();
	}
	if (m_xml->hasError())
		return Error::INVALID_FORMAT;
	return Error::OK;
}

}
