/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "DialSkinlet.h"
#include "Dial.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextColors.h>
#include <QskTextOptions.h>
#include <QskFunctions.h>

#include <QFontMetrics>
#include <QLineF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QtMath>

namespace
{
    class LinesNode : public QSGGeometryNode
    {
      public:
        LinesNode( int lineCount = 0 )
            : m_geometry( QSGGeometry::defaultAttributes_Point2D(), 2 * lineCount )
        {
            m_geometry.setDrawingMode( QSGGeometry::DrawLines );
            m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

            setGeometry( &m_geometry );
            setMaterial( &m_material );
        }

        void setColor( const QColor& color )
        {
            if ( color != m_material.color() )
            {
                m_material.setColor( color );
                markDirty( QSGNode::DirtyMaterial );
            }
        }

      private:
        QSGFlatColorMaterial m_material;
        QSGGeometry m_geometry;
    };

    class TicksNode : public LinesNode
    {
      public:
        TicksNode()
        {
        }
    };

    class NeedleNode : public LinesNode
    {
      public:
        NeedleNode()
            : LinesNode( 1 )
        {
        }

        void setData( const QLineF& line, qreal width )
        {
            auto vertexData = geometry()->vertexDataAsPoint2D();
            vertexData[ 0 ].set( line.x1(), line.y1() );
            vertexData[ 1 ].set( line.x2(), line.y2() );

            geometry()->setLineWidth( width );
            geometry()->markVertexDataDirty();

            markDirty( QSGNode::DirtyGeometry );
        }
    };
}

DialSkinlet::DialSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, NeedleRole, KnobRole, LabelsRole } );
}

QRectF DialSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subcontrol ) const
{
    QRectF r;

    if ( subcontrol == Dial::Knob )
    {
        const auto size = skinnable->strutSizeHint( Dial::Knob );
        r.setSize( size );
    }
    else
    {
        const auto extent = qMin( contentsRect.width(), contentsRect.height() );
        r.setSize( QSizeF( extent, extent ) );
    }

    r.moveCenter( contentsRect.center() );

    return r;
}

QSGNode* DialSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto speedometer = static_cast< const Dial* >( skinnable );

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( speedometer, node, Dial::Panel );

        case KnobRole:
            return updateBoxNode( speedometer, node, Dial::Knob );

        case NeedleRole:
            return updateNeedleNode( speedometer, node );

        case LabelsRole:
            return updateLabelsNode( speedometer, node );


        default:
            return nullptr;
    }
}

QSGNode* DialSkinlet::updateLabelsNode(
    const Dial* dial, QSGNode* node ) const
{
    using Q = Dial;

    const auto labels = dial->tickLabels();

    // ### actually, we could draw labels with only one entry
    if ( labels.count() <= 1 )
        return nullptr;

    auto ticksNode = static_cast< TicksNode* >( node );
    if ( ticksNode == nullptr )
        ticksNode = new TicksNode();

    const auto color = dial->color( Q::TickLabels );
    ticksNode->setColor( color );

    const auto startAngle = dial->minimum();
    const auto endAngle = dial->maximum();
    const auto step = ( endAngle - startAngle ) / ( labels.count() - 1 );

    auto geometry = ticksNode->geometry();
    geometry->allocate( labels.count() * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();

    auto scaleRect = this->scaleRect( dial );

    const auto center = scaleRect.center();
    const auto radius = 0.5 * scaleRect.width();

    const auto spacing = dial->spacingHint( Q::TickLabels );

    const auto font = dial->effectiveFont( Q::TickLabels );
    const QFontMetricsF fontMetrics( font );

    auto angle = startAngle;

    const auto tickSize = dial->strutSizeHint( Q::TickLabels );
    const auto needleRadius = radius - tickSize.height();

    // Create a series of tickmarks from minimum to maximum

    auto labelNode = ticksNode->firstChild();

    for ( int i = 0; i < labels.count(); ++i, angle += step )
    {
        const qreal cos = qFastCos( qDegreesToRadians( angle ) );
        const qreal sin = qFastSin( qDegreesToRadians( angle ) );

        const auto xStart = center.x() + radius * cos;
        const auto yStart = center.y() + radius * sin;

        const auto xEnd = center.x() + needleRadius * cos;
        const auto yEnd = center.y() + needleRadius * sin;

        vertexData[ 0 ].set( xStart, yStart );
        vertexData[ 1 ].set( xEnd, yEnd );

        vertexData += 2;

        const auto& text = labels.at( i );

        if ( !text.isEmpty() )
        {
            const auto w = qskHorizontalAdvance( fontMetrics, text );
            const auto h = fontMetrics.height();
            const auto adjustX = ( -0.5 * cos - 0.5 ) * w;
            const auto adjustY = ( -0.5 * sin - 0.5 ) * h;

            const auto numbersX = xEnd - ( spacing * cos ) + adjustX;
            const auto numbersY = yEnd - ( spacing * sin ) + adjustY;

            const QRectF numbersRect( numbersX, numbersY, w, h );

            labelNode = QskSkinlet::updateTextNode(
                dial, labelNode, numbersRect, Qt::AlignCenter | Qt::AlignHCenter,
                text, font, QskTextOptions(), QskTextColors( color ), Qsk::Normal );

            if ( labelNode )
            {
                if ( labelNode->parent() != ticksNode )
                    ticksNode->appendChildNode( labelNode );

                labelNode = labelNode->nextSibling();
            }
        }
    }

    geometry->setLineWidth( tickSize.width() );
    geometry->markVertexDataDirty();

    ticksNode->markDirty( QSGNode::DirtyGeometry );

    return ticksNode;
}

QSGNode* DialSkinlet::updateNeedleNode(
    const Dial* dial, QSGNode* node ) const
{
    using Q = Dial;

    auto needleNode = static_cast< NeedleNode* >( node );
    if ( needleNode == nullptr )
        needleNode = new NeedleNode();

    const auto line = needlePoints( dial );
    const auto width = dial->metric( Q::Needle | QskAspect::Size );

    needleNode->setData( line, width * 2 );
    needleNode->setColor( dial->color( Q::Needle ) );

    return needleNode;
}

QRectF DialSkinlet::scaleRect( const Dial* dial ) const
{
    using Q = Dial;

    const auto margins = dial->marginHint( Q::Panel );

    auto r = dial->subControlRect( Q::Panel );
    r = r.marginsRemoved( margins );

    return r;
}

QLineF DialSkinlet::needlePoints( const Dial* dial ) const
{
    const auto r = scaleRect( dial );
    const auto margin = dial->metric( Dial::Needle | QskAspect::Margin );

    QLineF line;
    line.setP1( r.center() );
    line.setLength( 0.5 * r.width() - margin );
    line.setAngle( -dial->value() );

    return line;
}

#include "moc_DialSkinlet.cpp"
