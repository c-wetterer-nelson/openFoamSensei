/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::sensei::senseiInput

Description
    An abstract input (source) for Paraview sensei.

See also
    Foam::sensei::dataQuery
    Foam::sensei::timeQuery

SourceFiles
    senseiInput.C

\*---------------------------------------------------------------------------*/

#ifndef sensei_senseiInput_H
#define sensei_senseiInput_H

#include "className.H"
#include "polyMesh.H"
#include "senseiTools.H"
#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declarations
class dictionary;
class mapPolyMesh;
class Time;

namespace sensei
{

/*---------------------------------------------------------------------------*\
                   Class sensei::senseiInput Declaration
\*---------------------------------------------------------------------------*/

class senseiInput
{
    // Private data

        //- The sensei input channel name
        word name_;

public:

    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            senseiInput,
            dictionary,
            (
                const word& name,
                const Time& runTime,
                const dictionary& dict
            ),
            (name, runTime, dict)
        );


    // Selectors

        //- Return a reference to the selected input
        static autoPtr<senseiInput> New
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );


    //- Runtime type information
    ClassName("sensei::input");


    // Constructors

        //- Construct with given sensei input channel name
        explicit senseiInput(const word& channel);


    //- Destructor
    virtual ~senseiInput() = default;


    // Member Functions

        //- The name of the sensei input channel
        virtual const word& name() const
        {
            return name_;
        }

        //- Read the specification
        virtual bool read(const dictionary& dict);

        //- Update for changes of mesh or mesh point-motion
        virtual void update(polyMesh::readUpdateState state);

        //- Add channels (with fields) to data query
        virtual label addChannels(dataQuery& dataq) = 0;

        //- Convert channels to vtkMultiBlockDataSet outputs
        virtual bool convert(dataQuery& dataq, outputChannels& outputs) = 0;


        //- Print information
        virtual Ostream& print(Ostream& os) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace sensei
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
