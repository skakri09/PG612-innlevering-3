Alle subtasks er løst i hennhold til kravene i mappen. Prosjektet har egne klasser for de forskjellige effektene som skulle lages. 

Prosjektet har følgende effekter:
-Coloreffect (ensfarget objekt)
-PhongEffect (phong farget object)
-ShadedPhongEffect (lik phong som over, men med skygger)
-ReflectEffect (reflective objekter. Absorberer 5% lys per reflect)
-ReflectDarekEdges (samme som over, men gjør kantene mørkere så spheres blir synlig om man ikke har cubemap
-FresnelEffect (fresnel effekt med snells law. Har implementert ca 250 forskjellige refractive indices)

Det er implementert ray traced cubemap med bilinear interpolation

Konstant maximum traversal depth er byttet ut med color contribution system hvor hver ray har en color contribution verdi. Hvis verien når 0.02(2%) eller mindre avsluttes raytracing og svart farge returneres for den gjeldende (sub-)rayen.

Har implementert 2x2 multi-sampling. Også lagt til et par andre multi-sample patterns 
(4x4(16 rays per pixel) og 8x8 (64 rays per pixel)). Man kan enkelt velge hvilken multi-sample pattern man vil bruke ved å bytte funksjonskallet i render() funksjonen.

Det er implementert ray tracing mot triangler. Her er den barycentriske metoden brukt fremfor half space testing.

Q: Render a scene with at least two reflective spheres: what is the difference to the maximum depth approach
A: Forskjellen avhenger av omstendighetene. Med lav oppløsning og max dybde på 7 ser man ingen forskjell på max depth av 7 og spheres som f.eks
absorberer 5% av "lyset". Skrur man derimot opp oppløsningen mye, og zoomer inn på refleksjonen, ser man at max dybden 7 nåes raskere enn farge contribution mindre enn 2%. Det blir nemlig en svart refleksjon raskere med max dybde enn color contribution. Fordelen med color contribution 
avsluttning fremfor max dybde er at man sørger for å ikke få et synlig svart område i f.eks refleksjonen

I tillegg til mandatory oppgaver er det implementert:
-Point-light Skygger for phong-shadede objekter
-plane sceneObject, lar deg definere et kvadratisk plan i scenen.