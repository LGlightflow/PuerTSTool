import * as UE from "ue";
import { blueprint } from "puerts";
import Misc from "../Misc/Misc";
export default function mixin(blueprintPath: string, objectTakeByNative = false) {
    return function <T extends UE.Object>(target: new () => T) {
        const ucls = UE.Class.Load(blueprintPath);
        
        const tsSubsys = Misc.GetTSSubsys();
        if (tsSubsys) {
            tsSubsys.CacheClass(ucls);
        }

        const BlueprintClass = blueprint.tojs(ucls);
        return blueprint.mixin(BlueprintClass, target, { objectTakeByNative }) as any;
    };
}
