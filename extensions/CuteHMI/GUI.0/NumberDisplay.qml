import QtQuick 2.5

import CuteHMI.GUI 0.0

/**
  Number display.

  Number display can be used to conveniently display numerical values with corresponding units of measurement. Number display tries
  to keep constant width. If that's not possible it expands to fit the content.
  */
Element
{
	id: root

	implicitWidth: background.width - valueDisplay.overfull
	implicitHeight: background.height

	active: value === value		// NaN === NaN equals false, thus NaN value deactivates display.

	/**
	  Font. Font that is used by the display. It may be desirable to set some monospace font.
	  */
	property alias font: valueDisplay.font

	/**
	  Left padding.
	  */
	property real leftPadding: font.pixelSize * 0.25

	/**
	  Right padding.
	  */
	property real rightPadding: font.pixelSize * 0.25

	/**
	  Top padding.
	  */
	property real topPadding: font.pixelSize * 0.25

	/**
	  Bottom padding.
	  */
	property real bottomPadding: font.pixelSize * 0.25

	/**
	  Value.
	  */
	property real value: NaN

	/**
	  Fractional width.
	  */
	property int fractionalWidth: 1

	/**
	  Integral width.
	  */
	property int integralWidth: 3

	/**
	  Unit of measurment.
	  */
	property string unit: "°C"

	/**
	  Text formatter.
	  */
	property var textFormatter: function(value) { return value.toFixed(root.fractionalWidth) }

	Rectangle {
		id: background

		x: -0.5 * valueDisplay.overfull
		width: contentItem.width + leftPadding + rightPadding
		height: contentItem.height + topPadding + bottomPadding

		radius: height / 5
		color: root.color.background
		border.color: root.color.stroke
		border.width: units.strokeWidth
	}

	Row {
		id: contentItem

		x: leftPadding - 0.5 * valueDisplay.overfull
		y: topPadding

		spacing: root.font.pixelSize * 0.25

		Text {
			id: valueDisplay

			width: Math.max(contentWidth, nominaLWidth)

			text: textFormatter(root.value)
			color: root.color.foreground
			horizontalAlignment: Text.AlignRight
			font.pixelSize: units.quadrat * 0.25
			font.family: CuteApplication.theme.fonts.monospace.family

			property real overfull: width - nominaLWidth
			property real nominaLWidth: contentWidth / text.length * (root.fractionalWidth + root.integralWidth + 1)
		}

		Text {
			id: unitDisplay

			text: root.unit
			font: root.font
			color: root.color.foreground
		}
	}
}

//(c)C: Copyright © 2020, Michał Policht <michal@policht.pl>. All rights reserved.
//(c)C: This file is a part of CuteHMI.
//(c)C: CuteHMI is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//(c)C: CuteHMI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//(c)C: You should have received a copy of the GNU Lesser General Public License along with CuteHMI.  If not, see <https://www.gnu.org/licenses/>.
